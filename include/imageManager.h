
#include <type_traits>
#include <optional>
#include "image.h"
#include "ierror.h"
#include "bhead.h"

enum class ECodes{noError, cantOpen, cantSave};

/*
  ImageManager
*/
template<typename Img>
class ImageManager : public IError<ECodes>{
  static_assert(std::is_base_of<IImage, Img>::value,
		"Template type must implement IImage interface.");
public:
  using OptImage = std::optional<Img>;
  
  ImageManager();  
  OptImage openImage(const std::string path);
  OptImage saveImage(const Img &img, const std::string path);
  OptImage saveImage(const std::string path, const Img &img);
};

#include "imageManager.tpp"
