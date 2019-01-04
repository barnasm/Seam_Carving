#include "bhead.h"
/*
  IImage
*/

class IImage{
protected:
public:
  using _ImageIpl = void;
};

/*
  ImageGIL
*/
template<typename ImageIpl = boost::gil::rgb8_image_t>
class ImageGIL : public IImage{
private:
public:
  ImageIpl m_img;
  using _ImageIpl = ImageIpl;
public:
  ImageGIL() = default;
  ImageGIL(ImageIpl &&i) : m_img(i) {}
  operator ImageIpl&() { return m_img; }
  //TODO view() cast
};


/*
  test template inheritance
*/
// class A{ public: virtual int add(int a, int b){return a+b;} };
// class B{ public: virtual int mul(int a, int b){return a*b;} };

// template<typename... Interfaces>
// class Test : public Interfaces... {
//   public:
//   //foo(Interfaces... ifaces) : Interfaces(ifaces)... {}

//   };
//   // Test<A,B> test;
//   // std::cout << test.add(1,2) << std::endl;
//   // std::cout << test.mul(1,2) << std::endl;
