#ifndef __DAC_H
#define __DAC_H


class inDAC {
  
public :
    inDAC() {}
    
    void initChan4();
    void initChan5();
    
    void initTim4();
    
    void initDMA4();
    
public :

};


extern inDAC dac;

#endif  // __DAC_H

