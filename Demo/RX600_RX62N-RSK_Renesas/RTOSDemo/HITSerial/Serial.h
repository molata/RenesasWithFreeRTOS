#ifndef HIT_SCI_API
#define HIT_SCI_API

/****** init sci ********/
void sci6_init(void);

/****** allow sci re interrupt *******/
void sci6_allowSCIRXI(void);

/****** allow sci te interrupt: default allow TEI interrupt ******/
void sci6_allowSCITXI(void);

/******** allow be-direction communication *******/
void sci6_allowSCIBECommunication();

/****** clear sci re status ********/
void sci6_clearREFault(void);
#endif