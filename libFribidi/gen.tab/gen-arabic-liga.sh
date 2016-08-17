#!/bin/sh

ARABICSHAPING=unidata/ArabicShaping.txt
UNICODEDATA=unidata/UnicodeData.txt

LAMclass="(`grep '; LAM$' "$ARABICSHAPING" | cut -d';' -f1 | sort | tr '\n' '|' | sed 's/|$//'`)"
ALEFclass="(`grep '; ALEF$' "$ARABICSHAPING" | cut -d';' -f1 | sort | tr '\n' '|' | sed 's/|$//'`)"

grep -E ";<(final|isolated)> $LAMclass $ALEFclass;" "$UNICODEDATA"
