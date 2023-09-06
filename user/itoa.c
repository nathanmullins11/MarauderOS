#include <string.h>


char* itoa(int value, char* destination, int base)
{
    // handle if integer is just 0
    if (value == 0)
    {
        destination[0] = '0';
        destination[1] = '\0';
        return destination;
    }

    int i = 0;
    char temp[50];
    // loop to get converted string until value is 0
    while (value != 0)
    {
        // get remainder of value
        int remainder = value % base;
        // check if remainder > 10 - will not fit in one index (only when desired base is >= 10)
        if (remainder >= 10)
        {
            temp[i] = remainder - 10 + 'A';
        }
        else
        {
            // set remainder to index in temp of string to output
            temp[i] = remainder + '0';
        }
        // set new value 
        value = value / base;
        i++;
    }

    // end temp array
    temp[i] = '\0';

    // now string exists in temp in reverse order
    // need to get the order correct

    // first set length of temp array
    int length = i;

    // set last element in temp to first element in destination array until done
    for (int index = 0; index < length; index++)
    {
        destination[index] = temp[length - index - 1];
    }

    destination[length] = '\0';

    return destination;
    
}
