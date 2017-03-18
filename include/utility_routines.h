

#ifndef __UTILITY_ROUTINES_H
#define __UTILITY_ROUTINES_H

//*****************************************************************************

unsigned int domain_list_to_bit_mask(const char* domain_list)
{
   unsigned int bit_mask = 0;
   char* token;
   char* domain_list_copy = strdup(domain_list);
   char* rest = domain_list_copy;
   int i;
   
   while ((token = strtok_r(rest, ",", &rest))) {
     for (i = 0; i != END_DOMAINS; ++i) {
       if (!strcmp(token, domains_names[i])) {
	   bit_mask |= (1 << i);
	   break;
	 }
     }
     if (!strcmp(token, "ALL")) {
	 bit_mask |= (~0);
       }
       
   }
   free(domain_list_copy);

   return bit_mask;
}

#endif
