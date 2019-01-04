
#include <type_traits>
#include <optional>
#include "image.h"
#include "ierror.h"
#include "bhead.h"

enum class ECodes{noError, cantOpen, cantSave};

/*
  IImageManager
*/
template<typename Img = IImage>
class IImageManager{
  static_assert(std::is_base_of<IImage, Img>::value,
		"Template type must implement IImage interface.");
public:
  using OptImage = std::optional<Img>;
  
public:
  virtual OptImage openImage(const std::string path)=0;
  virtual OptImage saveImage(const Img &img, const std::string path)=0;
  virtual OptImage saveImage(const std::string path, const Img &img)=0;
};

/*
  my_check
*/
namespace my_check{
  template <typename T, typename = void> struct has_castop : std::false_type {};
  template <typename T>
    struct has_castop<
      T
    , std::void_t<decltype(std::declval<T>().operator typename T::_ImageIpl&())>>
    : std::true_type {};

  template<class T>
  decltype(std::declval<T>().operator typename T::_ImageIpl&(), std::true_type{})
  has_castop2(){ return {}; }
  auto has_castop2(...) { return std::false_type {}; }
  
  template<typename T> using has_castop_t = typename has_castop<T>::type;
  template<typename T> const auto has_castop_v = has_castop<T>::value;
  template<typename T> using en_if_t = std::enable_if_t< my_check::has_castop_v<T>>;
}

/*
  ImageManagerBoost
*/
template<typename Img = ImageGIL<>>
class ImageManagerBoost
  : public IError<ECodes>
  , public IImageManager<Img>
{
  static_assert(std::is_constructible_v<Img, typename Img::_ImageIpl &&>,
  		"Template type must be constructable with rvalue &&.");
  static_assert(my_check::has_castop_v<Img>,
		"Template type has to implement cast operator."); 

private:  
  using ImageIplR = typename Img::_ImageIpl&;
public:
  using OptImage = typename IImageManager<Img>::OptImage;
  
  ImageManagerBoost();  
  OptImage openImage(const std::string path);
  OptImage saveImage(const Img &img, const std::string path);
  OptImage saveImage(const std::string path, const Img &img);
};


#include "imageManager.tpp"
