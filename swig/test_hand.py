'''
libqp test script
'''
import libqp

print 'test hand and card:'
cd = libqp.card_t()
cd.suit = libqp.cdSuitHeart
cd.rank = libqp.cdRankAce
h = libqp.hand_new(20)
print 'push'  a card to hand'
libqp.hand_push(h,cd)
cd.rank = libqp.cdRank2
print 'push a card to hand'
libqp.hand_push(h,cd)

print 'print a hand'
libqp.hand_print(h,10)
