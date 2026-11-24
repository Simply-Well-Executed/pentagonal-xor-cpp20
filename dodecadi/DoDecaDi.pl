# SPDX-License-Identifier: 0BSD
# DODECADI-001: local Perl 5.44.0, no use/require, modules, downloads or globals.
my $value = 0;
my $count = 0;
while ($value <= 255) {
    my $shell = 0x96 ^ (0x69 ^ $value);
    my $expected = 255 ^ $value;
    die "DODECADI|HOLD|xor\n" unless $shell == $expected;
    my $original = 0x69 ^ (0x96 ^ $shell);
    die "DODECADI|HOLD|inverse\n" unless $original == $value;
    $value = $value + 1;
    $count = $count + 1;
}
my $a = 0x96 ^ (0x69 ^ 219);
my $b = 0x96 ^ (0x69 ^ 223);
die "DODECADI|HOLD|canary\n" if $a == $b || ($a | 0x24) != ($b | 0x24);
die "DODECADI|HOLD|count\n" unless $count == 256;
print "DODECADI|PASS|LAPS=256|NO_MODULES|RELEASE=HOLD\n";
