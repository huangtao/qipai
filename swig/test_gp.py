'''
libqp test script
'''
import libqp

print 'test gp hand type'
gp = libqp.gp_new()
h = libqp.hand_new(20)
ht = libqp.hand_type()

libqp.hand_from_string(h,"D3");
libqp.gp_handtype(h,ht)
libqp.hand_print(h,10)
print libqp.gp_htype_name(ht)

libqp.hand_from_string(h,"D3,S3");
libqp.gp_handtype(h,ht)
libqp.hand_print(h,10)
print libqp.gp_htype_name(ht)

libqp.hand_from_string(h,"D3,S3,H3");
libqp.gp_handtype(h,ht)
libqp.hand_print(h,10)
print libqp.gp_htype_name(ht)

libqp.hand_from_string(h,"D3,S4,H6,H5,C7");
libqp.gp_handtype(h,ht)
libqp.hand_print(h,10)
print libqp.gp_htype_name(ht)

libqp.hand_from_string(h,"D3,S3,D4,S4,D6,S5,C5,C6");
libqp.gp_handtype(h,ht)
libqp.hand_print(h,10)
print libqp.gp_htype_name(ht)

libqp.hand_from_string(h,"DJ,SJ,CJ,CQ,HQ,SQ");
libqp.gp_handtype(h,ht)
libqp.hand_print(h,10)
print libqp.gp_htype_name(ht)

libqp.hand_from_string(h,"DJ,SJ,CJ,CQ,HQ,SQ,D6,D7,S6,H7");
libqp.gp_handtype(h,ht)
libqp.hand_print(h,10)
print libqp.gp_htype_name(ht)

libqp.hand_from_string(h,"DJ,SJ,CJ,HJ,D6,S6,H7");
libqp.gp_handtype(h,ht)
libqp.hand_print(h,10)
print libqp.gp_htype_name(ht)

libqp.hand_from_string(h,"D5,S5,C5,H5,HQ");
libqp.gp_handtype(h,ht)
libqp.hand_print(h,10)
print libqp.gp_htype_name(ht)

libqp.hand_from_string(h,"SK,HK,CK,D3");
libqp.gp_handtype(h,ht)
libqp.hand_print(h,10)
print libqp.gp_htype_name(ht)

