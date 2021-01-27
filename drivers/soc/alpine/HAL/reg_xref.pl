#!/usr/bin/perl

$reg_file_name = $ARGV[0];
$top_struct_name = $ARGV[1];
$top_struct_instance_name = $ARGV[2];

$depth = 0;

open(reg_file, "<$reg_file_name") or die "Unable to open register file \"$reg_file_name\"!";

sub handle_struct_definition {
	local $name = shift;

#	local $i;
#	for ($i = 0; $i < $depth; $i++) {
#		printf("\t");
#	}
#	printf("%s\n", $1);

	$root_structs{$1}->{name} = $name;
	$root_structs{$1}->{type} = "struct";
	$root_structs{$1}->{struct_obj} = \$root_structs{$name};
	$root_structs{$1}->{struct_name} = $name;
	$root_structs{$1}->{exist} = 1;
	$root_structs{$1}->{num_children} = 0;

	$current = \$root_structs{$1};
}

sub handle_struct_instance {
	local $type_name = shift;
	local $name = shift;

#	local $i
#	for ($i = 0; $i < $depth; $i++) {
#		printf("\t");
#	}
#	printf("%s %s\n", $type_name, $name);

	local $new;
	$new->{name} = $name;
	$new->{type} = "struct";
	$new->{struct_obj} = \$root_structs{$type_name};

	$$current->{children}[$$current->{num_children}] = \$new;
	$$current->{num_children}++;
}

sub handle_scalar_instance {
	local $name = shift;

	if ($name =~ "rsrvd") {
		next;
	}

	if ($name =~ "resvd") {
		next;
	}

	if ($name =~ "reserved") {
		next;
	}

	if ($name =~ "Reserved") {
		next;
	}

#	for ($i = 0; $i < $depth; $i++) {
#		printf("\t");
#	}
#	printf("%s\n", $name);

	local $new;
	$new->{name} = $name;
	$new->{type} = "scalar";

	$$current->{children}[$$current->{num_children}] = \$new;
	$$current->{num_children}++;
}

while (<reg_file>)
{
	chomp;

	if (($depth == 0) && ($_ =~ /^\s*struct\s*([_a-zA-Z0-9]*)\s*{.*/)) {
		handle_struct_definition($1);
		$depth++;
	} elsif (($depth == 0) && ($_ =~ /^\s*union\s*([_a-zA-Z0-9]*)\s*{.*/)) {
		handle_struct_definition($1);
		$depth++;
	} elsif ($_ =~ /^\s*struct\s\s*([_a-zA-Z0-9]*)\s\s*([_a-zA-Z0-9]*)\s*;.*/) {
		handle_struct_instance($1, $2);
	} elsif ($_ =~ /^\s*struct\s\s*([_a-zA-Z0-9]*)\s\s*__iomem\s\s*([_a-zA-Z0-9]*)\s*;.*/) {
		handle_struct_instance($1, $2);
	} elsif ($_ =~ /^\s*union\s\s*([_a-zA-Z0-9]*)\s\s*([_a-zA-Z0-9]*)\s*;.*/) {
		handle_struct_instance($1, $2);
	} elsif ($_ =~ /^\s*struct\s\s*([_a-zA-Z0-9]*)\s\s*([_a-zA-Z0-9]*)\[(.*)\]\s*;.*/) {
		local $i;
		for ($i = 0; $i < $3; $i++) {
			handle_struct_instance($1, $2 . "[$i]");
		}
	} elsif ($_ =~ /^\s*union\s\s*([_a-zA-Z0-9]*)\s\s*([_a-zA-Z0-9]*)\[(.*)\]\s*;.*/) {
		local $i;
		for ($i = 0; $i < $3; $i++) {
			handle_struct_instance($1, $2 . "[$i]");
		}
	} elsif ($_ =~ /^\s*uint16_t\s\s*([_a-zA-Z0-9]*)\s*;.*/) {
		handle_scalar_instance($1);
	} elsif ($_ =~ /^\s*uint16_t\s\s*([_a-zA-Z0-9]*)\[(.*)\]\s*;.*/) {
		local $i;
		for ($i = 0; $i < $2; $i++) {
			handle_scalar_instance($1 . "[$i]");
		}
	} elsif ($_ =~ /^\s*uint32_t\s\s*([_a-zA-Z0-9]*)\s*;.*/) {
		handle_scalar_instance($1);
	} elsif ($_ =~ /^\s*uint32_t\s\s*([_a-zA-Z0-9]*)\[(.*)\]\s*;.*/) {
		local $i;
		for ($i = 0; $i < $2; $i++) {
			handle_scalar_instance($1 . "[$i]");
		}
	} elsif (($depth > 0) && ($_ =~ /^\s*}.*/)) {
		$depth--;
	}
}

#printf("#######################################\n");

if ($root_structs{$top_struct_name}->{exist} != 1) {
	die "Unable to find top struct: $top_struct_name!\n";
}

if ($root_structs{$top_struct_name}->{num_children} < 1) {
	die "Top struct has no children!\n";
}

sub print_obj {
	local $obj = shift;
	local $prefix = shift;
	local $i;

#	printf("%s (%s) has %u children\n", $obj->{name}, $obj->{type}, $obj->{num_children});
	if ($obj->{type} eq "struct") {
		local $new_prefix;

		if ($obj->{name} eq $top_struct_name) {
			$new_prefix = "$prefix" . "&$top_struct_instance_name->";
		} else {
			$new_prefix = "$prefix" . "$obj->{name}.";
		}

		$obj = $obj->{struct_obj};

		for ($i = 0; $i < $$obj->{num_children}; $i++) {
			local $child = $$obj->{children}[$i];
			if ($$child->{type} == "struct") {
				$child_struct_obj = $$child->{struct_obj};
				print_obj($$child, $new_prefix);
			}
		}
	} elsif ($obj->{type} eq "scalar") {
		printf("\tprintf(\"%s%s = %%p\\n\", %s%s);\n", $prefix, $obj->{name}, $prefix, $obj->{name});
	}
}

printf("#include \"stdio.h\"\n");
printf("#include \"%s\"\n", $reg_file_name);
printf("\n");
printf("void reg_print_offsets_%s(void *base_addr)\n", $top_struct_instance_name);
printf("{\n");
printf("\tstruct %s *%s = (struct %s *)(base_addr);\n", $top_struct_name, $top_struct_instance_name, $top_struct_name);
print_obj($root_structs{$top_struct_name}, "");
printf("}\n");

