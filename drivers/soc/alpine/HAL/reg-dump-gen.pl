#!/bin/perl

use File::Basename;
$script_name = basename($0);

####################################################################################################
# Argument handling
####################################################################################################
if (@ARGV != 2) {
	print "Illegal number of parameters!\n";
	print "\n";
	print "Usage:\n";
	print "$script_name <profile file> <target type>\n";
	print "\n";
	print "profile file:\n";
	print "- Profile of the platform for which register dump is required\n";
	print "- Lines beginning with '#' are ignored\n";
	print "- Regular expressions are supported\n";
	print "- Example syntax:\n";
	print "  # Architecture register file\n";
	print "  REG_FILE=reg_xref_alpine_v2.txt\n";
	print "  # Architecture exclusions\n";
	print "  EXCLUDE_FILE=reg_xref_alpine_v2_exclude.txt\n";
	print "  # Platform specific exclusions\n";
	print "  EXCLUDE_SINGLE=^eth[13]\n";
	print "  EXCLUDE_SINGLE=^pcie[123]\n";
	print "target type:\n";
	print "- c - c include file\n";
	print "- devmem2 - devmem2 commands\n";
	print "- al-tool - al_tool commands\n";
	print "- uboot - U-Boot script\n";
	print "- raw - Raw output\n";
	print "\n";
	print "Example - Prepare a C include file for dumping all Alpine V2 EVP related regs:\n";
	print "perl $script_name reg-dump-gen-profile-alpine-v2-evp.cfg c > reg-dump-gen-alpine-v2-evp.h\n";
	print "\n";
	exit -1;
}

$profile_file_name=$ARGV[0];
#print "profile_file_name=$profile_file_name\n";
shift;
$target=$ARGV[0];
#print "target=$target\n";
shift;

####################################################################################################
# Parse profile file
####################################################################################################
open $profile_file, $profile_file_name or die "Could not open $profile_file_name: $!";
$num_reg_files=0;
$num_exclude_files=0;
$num_unit_includes=0;
$num_exclude_singles=0;
while( my $line = <$profile_file>) {
	chomp $line;
	if ($line eq "") {
	} elsif ($line =~ "^#.*") {
	} elsif ($line =~ "^REG_FILE=\(.*\)") {
		if ($num_reg_files > 0) {
			die;
		}
		$reg_file_name=$1;
		$num_reg_files++;
	} elsif ($line =~ "^INCLUDE_FILE=\(.*\)") {
		$include_files[$num_include_files]=$1;
		$num_include_files++;
	} elsif ($line =~ "^EXCLUDE_FILE=\(.*\)") {
		$exclude_files[$num_exclude_files]=$1;
		$num_exclude_files++;
	} elsif ($line =~ "^INCLUDE_SINGLE=\(.*\)") {
		$include_singles[$num_include_singles]=$1;
		$num_include_singles++;
	} elsif ($line =~ "^EXCLUDE_SINGLE=\(.*\)") {
		$exclude_singles[$num_exclude_singles]=$1;
		$num_exclude_singles++;
	} else {
		die "Cannot handle \"$line\"";
	}
}
close $profile_file;

####################################################################################################
# Parse include files
####################################################################################################
for ($i = 0; $i < $num_include_files; $i++) {
	open $include_file, $include_files[$i] or die "Could not open $include_files[$i]: $!";
	while( my $line = <$include_file>) {
		chomp $line;
		if ($line eq "") {
			next;
		}
		if ($line =~ "^#.*") {
			next;
		}
		$include_singles[$num_include_singles]=$line;
		$num_include_singles++;
	}
	close $include_file;
}

####################################################################################################
# Parse exclude files
####################################################################################################
for ($i = 0; $i < $num_exclude_files; $i++) {
	open $exclude_file, $exclude_files[$i] or die "Could not open $exclude_files[$i]: $!";
	while( my $line = <$exclude_file>) {
		chomp $line;
		if ($line eq "") {
			next;
		}
		if ($line =~ "^#.*") {
			next;
		}
		$exclude_singles[$num_exclude_singles]=$line;
		$num_exclude_singles++;
	}
	close $exclude_file;
}

####################################################################################################
# Parse reg file
####################################################################################################
open $reg_file, $reg_file_name or die "Could not open $reg_file_name: $!";
$num_regs=0;
while( my $line = <$reg_file>) {
	chomp $line;
	if ($line =~ "^\&\([^ ]*[^ ]*\) = \([^ ]*[^ ]*\)") {
		$reg[$num_regs]{name}=$1;
		$reg[$num_regs]{addr}=$2;

		if ($num_include_singles > 0) {
			$skip = 1;
		} else {
			$skip = 0;
		}

		for ($i = 0; $i < $num_include_singles; $i++) {
			if ($reg[$num_regs]{name} =~ $include_singles[$i]) {
				$skip = 0;
			}
		}

		for ($i = 0; $i < $num_exclude_singles; $i++) {
			if ($reg[$num_regs]{name} =~ $exclude_singles[$i]) {
				$skip = 1;
			}
		}

		if ($skip == 0) {
			$num_regs++;
		}
	} else {
		die;
	}
}
close $reg_file;

####################################################################################################
# Header
####################################################################################################
if ($target eq "c") {
	print "#ifndef __AL_REG_DUMP_XXX_H__\n";
	print "#define __AL_REG_DUMP_XXX_H__\n";
	print "\n";
	print "#if (AL_REG_DUMP_XXX_INCLUDE_REG_NAMES == 1)\n";
	print "#define AL_REG_DUMP_XXX_ENT(_name, _addr)	{ .name = _name, .addr = (uintptr_t)_addr }\n";
	print "#else\n";
	print "#define AL_REG_DUMP_XXX_ENT(_name, _addr)	{ .addr = (uintptr_t)_addr }\n";
	print "#endif\n";
	print "\n";
	print "struct reg_dump_xxx_ent {\n";
	print "#if (AL_REG_DUMP_XXX_INCLUDE_REG_NAMES == 1)\n";
	print "\tconst char *name;\n";
	print "#endif\n";
	print "\tuintptr_t addr;\n";
	print "};\n";
	print "\n";
	print "static const struct reg_dump_xxx_ent reg_dump_xxx_ent_arr[] = {\n";
} elsif ($target eq "devmem2") {
	print "#!/bin/bash\n";
	print "\n";
	print "set -e\n";
	print "\n";
} elsif ($target eq "al-tool") {
	print "#!/bin/bash\n";
	print "\n";
	print "set -e\n";
	print "\n";
}

####################################################################################################
# Body
####################################################################################################
for ($i=0; $i < $num_regs; $i++) {
	$reg_name=$reg[$i]{name};
	$reg_addr=$reg[$i]{addr};

	if ($target eq "raw") {
		print "$reg_name = $reg_addr\n";
	} elsif ($target eq "uboot") {
		print "echo \"$reg_name\"\n";
		print "md.l $reg_addr 1\n";
	} elsif ($target eq "devmem2") {
		print "echo \"$reg_name: \"\n";
		print "devmem2 $reg_addr w | grep Value\n";
	} elsif ($target eq "al-tool") {
		if (hex($reg_addr) > 0xfc000000) {
			$reg_offset=hex($reg_addr)-0xfc000000;
			print "echo \"$reg_name ($reg_addr): \"\n";
			printf("al_tool \$AL_TOOL_FLAGS reg 0x%08x w | grep Value\n", $reg_offset);
		}
	} elsif ($target eq "c") {
		print "\tAL_REG_DUMP_XXX_ENT(\"$reg_name\", $reg_addr),\n";
	}
}

#for al_tool we need reconfigure the atu and do another go at registers for system fabric registers
if ($target eq "al-tool") {
    print "\nbase_address_top=0x\$(lspci -vv \$AL_TOOL_FLAGS | grep \"Region 5\" | cut -dt -f2 | cut -d\\  -f2)\n";
    print "al_tool \$AL_TOOL_FLAGS apcea -c apcea_set_atu --base \${base_address_top} --target 0xf0000000 --size 0x1000000\n\n";

    for ($i=0; $i < $num_regs; $i++) {
            $reg_name=$reg[$i]{name};
            $reg_addr=$reg[$i]{addr};

            if ((hex($reg_addr) > 0xf0000000) && (hex($reg_addr) < 0xf1000000)) {
                   $reg_offset=hex($reg_addr)-0xf0000000;
                   print "echo \"$reg_name ($reg_addr): \"\n";
                   printf("al_tool \$AL_TOOL_FLAGS reg 0x%08x w | grep Value\n", $reg_offset);
            }

            #if there is a register that we don't cover - print
            if ((hex($reg_addr) < 0xf0000000) || (hex($reg_addr) > 0xfe000000) || 
                ((hex($reg_addr) > 0xf1000000) && (hex($reg_addr) < 0xfc000000)) ) {
                print STDERR "\nWARNING $reg_addr will not be dumped.\n"
            }
    }

    #return ATU to normal...
    print "al_tool \$AL_TOOL_FLAGS apcea -c apcea_set_atu --base \${base_address_top} --target 0xfc000000 --size 0x2000000\n\n";
}




####################################################################################################
# Footer
####################################################################################################
if ($target eq "c") {
	print "};\n";
	print "\n";
	print "static const unsigned int reg_dump_xxx_num_entries = sizeof(reg_dump_xxx_ent_arr) / sizeof(reg_dump_xxx_ent_arr[0]);\n";
	print "\n";
	print "#endif\n";
}

