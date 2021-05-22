#!/usr/bin/perl

sub cond_add_to_list {
	my $line = shift;
	my $regex = shift;
	my $list = shift;
	my $num_elements = @$list;

	if ($line =~ $regex) {
		@$list[$num_elements] = $line;
	}
}

sub print_list {
	my $headline = $_[0];
	my @unsorted_list = @{$_[1]};
	my @list = sort {lc($a) cmp lc($b)} @unsorted_list;

	printf("%s\n", $headline);
	if (@list > 0) {
		for (my $i = 0; $i < @list; $i++) {
			printf("%s\n", $list[$i]);
		}
	} else {
		printf("* N/A\n");
	}
}

if (@ARGV != 2) {
	printf("Invalid number of arguments!\n");
	printf("Syntax: git_log_diff.sh <old commit ID> <new commit ID>\n");
	exit -1;
}

$old_commit_id = shift @ARGV;
chomp($old_commit_id = `git rev-parse --short $old_commit_id`);
$new_commit_id = shift @ARGV;
chomp($new_commit_id = `git rev-parse --short $new_commit_id`);

$new = 0;
open(my $fh, '-|', "git log $old_commit_id...$new_commit_id") or die $!;
while (<$fh>) {
	chomp;
	if ($_ =~ "^commit ([0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f])[0-9a-f]*") {
		$new=1;
		$commit_id=$1;
	} elsif ($_ =~ "^Author:") {
	} elsif	($_ =~ "^Date:") {
	} elsif ($_ =~ "Change-Id:") {
	} elsif ($_ =~ "Signed-off-by") {
	} elsif ($_ =~ "^[ \t]*\$") {
	} else {
		if ($new) {
			$line = sprintf("*%s (%s)", $_, $commit_id);
		} else {
			$line = sprintf("**%s", $_);
		}

		$line =~ s/^\*\s\s*/* /g;
		$line =~ s/^\*\*\s\s*/** /g;
		$line =~ s/^\*\* \*\s/** /g;

		#printf("%s\n", $line);

		if ($new) {
			cond_add_to_list($line, "Bug Fix]", \@bug_fixes);
			cond_add_to_list($line, "Optimization]", \@optimizations);
			cond_add_to_list($line, "API Change]", \@api_changes);
			cond_add_to_list($line, "New Feature]", \@new_features);
			cond_add_to_list($line, "New Architecture Support]", \@new_features);
			cond_add_to_list($line, "Refactoring]", \@refactorings);
		}

		$new = 0;
	}
}
close($fh);

printf("==HAL Log Report==\n");
printf("* Branch: %s", `git rev-parse --abbrev-ref HEAD`);
printf("* Commit ID range: %s...%s\n\n", $old_commit_id, $new_commit_id);
print_list("===Bug Fixes===", \@bug_fixes);
print_list("===Optimizations===", \@optimizations);
print_list("===API Changes===", \@api_changes);
print_list("===New Features===", \@new_features);
print_list("===Refactorings===", \@refactorings);

