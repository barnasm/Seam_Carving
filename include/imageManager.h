
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
  //TODO check cast operator
public:
  using OptImage = std::optional<Img>;
  
public:
  virtual OptImage openImage(const std::string path)=0;
  virtual OptImage saveImage(const Img &img, const std::string path)=0;
  virtual OptImage saveImage(const std::string path, const Img &img)=0;
};


/*
  ImageManagerBoost
*/
template<typename Img = ImageGIL<>>
class ImageManagerBoost
  : public IError<ECodes>
  , public IImageManager<Img>
{
  static_assert(std::is_constructible<Img, typename Img::_ImageIpl &&>::value,
  		"Template type must be constructable with rvalue &&.");
  //TODO check cast operator
  using ImageIplR = typename Img::_ImageIpl&;
public:
  using OptImage = typename IImageManager<Img>::OptImage;
  
  ImageManagerBoost();  
  OptImage openImage(const std::string path);
  OptImage saveImage(const Img &img, const std::string path);
  OptImage saveImage(const std::string path, const Img &img);
};


#include "imageManager.tpp"
