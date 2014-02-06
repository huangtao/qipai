#include "ht_str.h"
#include <string.h>

/*
 * ht_string_split - divide a string into fields
 *
 */
 int ht_string_split(char* string, char* fields[], int nfields, char* sep)
 {
    char* p;
    char c,sepc,sepc2;
    int fn;
    char** fp;
    char* sepp;
    int trimtrail;
    
    if(!string || !fields || !sep)
        return 0;
        
    if(strlen(string) > 1024)
        return 0;
    
    p = string;
    sepc = sep[0];
    fp = fields;
    
    fn = nfields;
    for(;;){
        *fp++ = p;
        fn--;
        if(fn == 0)
            break;
        while((c = *p++) != sepc){
            if(c == '\0')
                return (nfields - fn);
        }
        *(p-1) = '\0';
    }
    
    /* we have overflowed the fields vector -- just count them */
    fn = nfields;
    for(;;){
        while((c = *p++) != sepc){
            if(c == '\0')
                return (fn);
        }
    }
    
    return 0;
}
