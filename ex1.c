#include <csoap.h>

#include <stdio.h>
#include <string.h>

const char *url = "http://redhat.com/register/rpc";
const char *urn = "urn:AddressFetcher";
/*const char *urn = "urn:connector_service";*/
const char *method = "getAddressFromName";
/*const char *method = "Login";*/

struct Phone
{
  int areaCode;
  char *exchange;
  char *number;
};

#define SOAP_TYPE_USER_PHONE "ns2:phone"

struct Phone* PhoneCreate()
{
  struct Phone* phone;

  phone = (struct Phone*)malloc(sizeof(struct Phone));
  phone->exchange = NULL;
  phone->number = NULL;

  return phone;
}

void PhoneFree(struct Phone* phone)
{
  if (phone == NULL) return;

  if (phone->exchange != NULL)
    free(phone->exchange);
  if (phone->number != NULL)
     free(phone->number);
}

void PhoneDump(struct Phone* phone)
{
  printf("+ Phone\n");
  if (phone == NULL) {
    printf("(null)\n");
    printf("+ End Phone \n");
    return;
  }

  printf("\tareaCode = %d\n", 
    phone->areaCode);
  printf("\texchange = %s\n", 
    phone->exchange?phone->exchange:"(null}");
  printf("\tnumber = %s\n", 
    phone->number?phone->number:"(null}");
  printf("+ End Phone \n");
}

struct Phone* PhoneDeserialize(HSOAPPARAM rootParam)
{
  HSOAPPARAM param;
  struct Phone *result;
  int size;

  result = PhoneCreate();

  param = SoapParamGetChildren(rootParam);

  if (param == NULL) {
    PhoneFree(result);
    return NULL;
  }

  if (!SoapParamTypeIs(param, SOAP_TYPE_INT)) {
    PhoneFree(result);
    return NULL;
  }
  
  result->areaCode = SoapParamGetInt(param);

  param = SoapParamGetNext(param);

  if (param == NULL) {
    PhoneFree(result);
    return NULL;
  }

  if (!SoapParamTypeIs(param, SOAP_TYPE_STRING)) {
    PhoneFree(result);
    return NULL;
  }
  
  size = SoapParamGetString(param, NULL);
  result->exchange = (char*)malloc(sizeof(char)*size+1);
  SoapParamGetString(param, result->exchange);

  param = SoapParamGetNext(param);

  if (param == NULL) {
    PhoneFree(result);
    return NULL;
  }

  if (!SoapParamTypeIs(param, SOAP_TYPE_STRING)) {
    PhoneFree(result);
    return NULL;
  }
  
  size = SoapParamGetString(param, NULL);
  result->number = (char*)malloc(sizeof(char)*size+1);
  SoapParamGetString(param, result->number);

  return result;
}


struct Address
{
  char *city;
  struct Phone *phone;
  char *state;
  char *streetName;
  int streetNum;
  int zip;
};

#define SOAP_TYPE_USER_ADDRESS "address"

struct Address* AddressCreate()
{
  struct Address* address;

  address = (struct Address*)malloc(sizeof(struct Address));
  address->city = NULL;
  address->phone = PhoneCreate();
  address->state = NULL;
  address->streetName = NULL;
}


void AddressFree(struct Address* address)
{
  if (address == NULL) return;

  if (address->city) 
    free(address->city);
  if (address->phone) 
    PhoneFree(address->phone);
  if (address->state) 
    free(address->state);
  if (address->streetName) 
    free(address->streetName);
}

void AddressDump(struct Address* address)
{
  printf("+ Address\n");
  if (address == NULL) {
    printf("(null)\n");
    printf("+ End Address \n");
    return;
  }

  printf("\tcity = %s\n", 
    address->city?address->city:"(null}");
  PhoneDump(address->phone);
  printf("\tstate = %s\n", 
    address->state?address->state:"(null}");
  printf("\tstreetName = %s\n", 
    address->streetName?address->streetName:"(null}");
  printf("\tstreetNum = %d\n", 
    address->streetNum);
  printf("\tzip = %d\n", 
    address->zip);
  printf("+ End Address \n");
}

struct Address *AddressDeserialize(HSOAPPARAM rootParam)
{
  HSOAPPARAM param;
  struct Address *result;
  int size;

  result = AddressCreate();

  param = SoapParamGetChildren(rootParam);

  if (param == NULL) {
    AddressFree(result);
    return NULL;
  }

  if (!SoapParamTypeIs(param, SOAP_TYPE_STRING)) {
    AddressFree(result);
    return NULL;
  }
  
  size = SoapParamGetString(param, NULL);
  result->city = (char*)malloc(sizeof(char)*size+1);
  SoapParamGetString(param, result->city);


  param = SoapParamGetNext(param);

  if (param == NULL) {
    AddressFree(result);
    return NULL;
  }

  if (!SoapParamTypeIs(param, SOAP_TYPE_USER_PHONE)) {
    AddressFree(result);
    return NULL;
  }

  result->phone = PhoneDeserialize(param);

  param = SoapParamGetNext(param);

  if (param == NULL) {
    AddressFree(result);
    return NULL;
  }

  if (!SoapParamTypeIs(param, SOAP_TYPE_STRING)) {
    AddressFree(result);
    return NULL;
  }
  
  size = SoapParamGetString(param, NULL);
  result->state = (char*)malloc(sizeof(char)*size+1);
  SoapParamGetString(param, result->state);

  param = SoapParamGetNext(param);

  if (param == NULL) {
    AddressFree(result);
    return NULL;
  }

  if (!SoapParamTypeIs(param, SOAP_TYPE_STRING)) {
    AddressFree(result);
    return NULL;
  }
  
  size = SoapParamGetString(param, NULL);
  result->streetName = (char*)malloc(sizeof(char)*size+1);
  SoapParamGetString(param, result->streetName);

  param = SoapParamGetNext(param);

  if (param == NULL) {
    AddressFree(result);
    return NULL;
  }

  if (!SoapParamTypeIs(param, SOAP_TYPE_INT)) {
    AddressFree(result);
    return NULL;
  }
  
  result->streetNum = SoapParamGetInt(param);

  param = SoapParamGetNext(param);

  if (param == NULL) {
    AddressFree(result);
    return NULL;
  }

  if (!SoapParamTypeIs(param, SOAP_TYPE_INT)) {
    AddressFree(result);
    return NULL;
  }
  
  result->zip = SoapParamGetInt(param);

  return result;
};


int main2(int argc, char *argv[])
{
  HSOAPURL url;

  url = SoapUrlCreate(argv[1]);
  SoapUrlDump(url);
  SoapUrlFree(url);

};

int main(int argc, char *argv[])
{
  HSOAPCALL call;
  HSOAPRES result;
  HSOAPPARAM res;
  HSOAPFAULT fault;
  struct Address* address;
  char *str;
 
  printf("\nStarting %s ... \n", argv[0]);

  /*  SoapInit(argc, argv);*/

  call = SoapCallCreate((argc>1)?argv[1]:url, urn, method);

  SoapCallAddStringParam(call, "name", "John B. Good");
/*  SoapCallAddStringParam(call, "passwd", "openpdm");*/
/*  SoapCallAddStringParam(call, "system", "imanconnector703@pdmdev2_ImanServer");*/
  /*SoapCallAddIntParam(call, "age", 24);*/
 
  if (result = SoapCallInvoke(call)) {
    if (fault = SoapResGetFault(result)) {
		SoapFaultPrint( stdout, fault);
    } else {
      printf("Checking result\n");
      res = SoapResGetParamChildren(result);
      printf("Result = %p\n", res);
      if (res != NULL) {
        address = AddressDeserialize(res);
        printf("Deserializing\n", res);
        AddressDump(address);
        AddressFree(address);
      } else {
        printf("Corrupt response!\n");
      }
    }
  } else {
    printf("Result is null\n");
  }
  
  return 0;

  /*
  if (SoapResIsFault(result)) {
  } else {
    res = SoapResGetFirstChild(result);
    comments = SoapParamGetStringValue(res);
   
  }
  */
}

/*

struct Person
{
  char *name;
  char *lastname;
  int age;
};


void SerializePerson(HSOAPPARAM parent, void* data)
{
  Person* person = (Person*)data;

  SoapParamCreateString(parent, "name", person->name);
  SoapParamCreateString(parent, "lastname", person->lastname);
  SoapParamCreateInt(parent, "", person->age);
}



int main2(int argc, char *argv[])
{
  HSOAPCALL call;
  HSOAPRES result;
  HSOAPPARAM res;
  Person person = {"Ferhat", "Ayaz", 24};
 
  SoapInit(argc, argv);

  call = SoapCallCreate(url, urn, method);

  SoapCallAddComplexParam(call, "person", "tns:person",  
			  SerializePerson, &person);

  result = SoapCallInvoke(call);

  if (SoapResIsFault(result)) {
  } else {
    res = SoapResGetFirstChild(result);
    comments = SoapParamGetStringValue(res);
   
  }
}

*/
