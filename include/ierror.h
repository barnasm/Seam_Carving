
#include <map>

/*
  IError
*/
enum class NoECodes{noError};
template<typename EC = NoECodes>
class IError{
  using strErr = std::string;
  EC eCode;
protected:
  std::map<EC, strErr> eMap;
  
public:
  IError(): eCode(EC::noError)
  {
    eMap[EC::noError] = "No errors occurred.";
  }

  strErr getEText(){ return eMap[eCode]; }
  bool isError() const {return eCode != EC::noError;}
  operator bool() const {return isError();}
  void setECode(const EC ec){ eCode = ec; }
};

