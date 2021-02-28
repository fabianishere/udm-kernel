#!/usr/bin/perl -w

my $first_line=`git log -1 --pretty=%B | head -1`;
my $error=0;

if ($first_line =~ /^[a-zA-Z0-9_ \.\/\:][a-zA-Z0-9_ \.\/\:]* (\[.*\]): .*/) {
	my $type = $1;

	if ($type =~ /\[Bug Fix\]/) {
	} elsif ($type =~ /\[Build Fix\]/) {
	} elsif ($type =~ /\[API Change\]/) {
	} elsif ($type =~ /\[New Architecture Support\]/) {
	} elsif ($type =~ /\[Optimization\]/) {
	} elsif ($type =~ /\[Refactoring\]/) {
	} elsif ($type =~ /\[Cosmetic Change\]/) {
	} elsif ($type =~ /\[New Feature\]/) {
	} elsif ($type =~ /\[Build System Change\]/) {
	} elsif ($type =~ /\[Jenkins Change\]/) {
	} elsif ($type =~ /\[Unit Test\]/) {
	} else {
		$error = 1;
	}
} elsif( $first_line =~ /Revert \".+\"/ ) {
} else {
	$error = 1;
}

if ($error == 1) {
	printf("*************************************************************\n");
	printf("*************************************************************\n");
	printf("Commit message check error!\n");
	printf("Commit message first line should be in the following format:\n");
	printf("unit subunit [Type of change]: short description\n\n");
	printf("Type of change can be one of the following:\n");
	printf("\t[Bug Fix]\n");
	printf("\t[Build Fix]\n");
	printf("\t[API Change]\n");
	printf("\t[New Architecture Support]\n");
	printf("\t[Optimization]\n");
	printf("\t[Refactoring]\n");
	printf("\t[Cosmetic Change]\n");
	printf("\t[New Feature]\n");
	printf("\t[Build System Change]\n");
	printf("\t[Jenkins Change]\n");
	printf("\t[Unit Test]\n");
	printf("\tRevert \"<other commit message>\"\n");
	printf("*************************************************************\n");
	printf("*************************************************************\n");
	exit(-1);
}
