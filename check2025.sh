#/bin/bash
rm -f a.out
set -e
clang -std=c17 -march=native -U_FORTIFY_SOURCE -ffunction-sections -fdata-sections -fintegrated-as -fintegrated-cc1 -fuse-ld=lld -faddrsig -Wl,--icf=all -Wl,--gc-sections  -fPIE -pie -O1  -gfull -Wall -Wextra -fno-diagnostics-show-option -fstandalone-debug -fvisibility=hidden -fstack-size-section -fstack-clash-protection -fstack-protector-all -ftrivial-auto-var-init=pattern -fsanitize=address,undefined,cfi -fsanitize=implicit-conversion -fsanitize=unsigned-integer-overflow -fsanitize=signed-integer-overflow -fsanitize=local-bounds -fno-common -fno-omit-frame-pointer -fsanitize-address-use-after-scope -fsanitize-address-use-after-return=always -fsanitize-address-use-odr-indicator -fno-optimize-sibling-calls -flto -fno-sanitize-recover=all -fno-plt -Wl,-z,relro,-z,now  -Wl,-z,noexecstack -Wtautological-compare -Wsign-compare -Wno-unused-result $1 -lm -lubsan
echo "＊＊＊ここまでがコンパイラの表示で以下は実行したときの表示です＊＊＊"
export ASAN_OPTIONS="detect_leaks=1:strict_init_order=1:detect_stack_use_after_return=1:check_initialization_order=1:strict_string_checks=1:detect_invalid_pointer_pairs=2:use_odr_indicator=1:abort_on_error=0" UBSAN_OPTIONS="print_stacktrace=1:abort_on_error=0"
./a.out "$@"
exit 0
