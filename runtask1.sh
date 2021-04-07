# ./sim -f page-blocked.ref -m 50  -s 104007 -a rand
# ./sim -f page-blocked.ref -m 50  -s 104007 -a fifo
# ./sim -f page-blocked.ref -m 50  -s 104007 -a clock
./sim -f page-simpleloop.ref -m 50  -s 104007 -a lru
# ./sim -f page-blocked.ref -m 50  -s 104007 -a mru