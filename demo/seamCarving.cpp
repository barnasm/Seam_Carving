#include "seamCarving.h"
#include <algorithm>
#include <memory>

SeamCarving::SeamCarving(Image &imageOriginal,
			 Image &imageEnergy,
			 Image &imageToDisplay,
			 std::shared_ptr<PixelEnergy> &energy){

  if(energy)
    this->energy = energy;
  else
    this->energy = std::make_shared<EnergySimple>();

  block = true;
  
  this->imageEnergy = &imageEnergy;
  this->imageToDisplay = &imageToDisplay;

  scImage.reset(new ScImage(imageOriginal.getWidth(), imageOriginal.getHeight()));
  //scImage = new ScImage(imageOriginal.getWidth(), imageOriginal.getHeight());
  currentWidth = imageOriginal.getWidth();
  
  for(std::size_t y = 0; y < scImage->getHeight(); y++){
    for(std::size_t x = 0; x < scImage->getWidth(); x++){
      scImage->setPixelColor(x, y, imageOriginal.getPixelColor(x, y));

      setNeighbours(x, y);
    
      (*scImage)[y][x].
	setEnergy(this->energy->computePixelEnergy(x, y, imageOriginal));
    }
  }
  createImageToShow();
}

void SeamCarving::setNeighbours(std::size_t x, std::size_t y){
  if((int)y-1 < 0){
    //(*scImage)[y][x].setNeighbour(&(*scImage)[y][x],   ScPixel::UP);
    (*scImage)[y][x].setNeighbour(&(*scImage)[y+1][x], ScPixel::DOWN); 
  }
  else if((int)y+1 >= scImage->getHeight()){
    (*scImage)[y][x].setNeighbour(&(*scImage)[y-1][x], ScPixel::UP);
    //(*scImage)[y][x].setNeighbour(&(*scImage)[y][x],   ScPixel::DOWN); 	
  }
  else{
    (*scImage)[y][x].setNeighbour(&(*scImage)[y-1][x], ScPixel::UP);
    (*scImage)[y][x].setNeighbour(&(*scImage)[y+1][x], ScPixel::DOWN);	
  }
  
  if((int)x-1 < 0){
    (*scImage)[y][x].setNeighbour(&(*scImage)[y][x+1], ScPixel::RIGHT);
    //(*scImage)[y][x].setNeighbour(&(*scImage)[y][x],   ScPixel::LEFT); 
  }
  else if(x+1 >= scImage->getWidth()){
    //(*scImage)[y][x].setNeighbour(&(*scImage)[y][x],   ScPixel::RIGHT);
    (*scImage)[y][x].setNeighbour(&(*scImage)[y][x-1], ScPixel::LEFT);
  }
  else{
    (*scImage)[y][x].setNeighbour(&(*scImage)[y][x+1], ScPixel::RIGHT);
    (*scImage)[y][x].setNeighbour(&(*scImage)[y][x-1], ScPixel::LEFT);
  }
}

void SeamCarving::computeImageEnergy(){
  for(std::size_t y = 0; y < imageEnergy->getHeight(); y++){
    for(std::size_t x = 0; x < imageEnergy->getWidth(); x++){
      (*scImage)[y][x].
	setEnergy(energy->computePixelEnergy(x, y, *scImage));
    }
  }
  createImageToShow();
}

void SeamCarving::sizeChanged(std::size_t width, std::size_t height){
  if(!scImage || block)
    return;
  
  if(currentWidth == width)
    return;
   
  if(currentWidth < width){
    currentWidth = std::min(scImage->getWidth(), width);
    
        //ScPixel *px;
    //px->getNeighbour(ScPixel::LEFT). setNeighbour(px->getNeighbour(ScPixel::RIGHT), ScPixel::RIGHT);
    //px->getNeighbour(ScPixel::RIGHT).setNeighbour(px->getNeighbour(ScPixel::LEFT),  ScPixel::LEFT);
    //return;
  }
  else{
    while((scImage->getWidth() - width) > seams.size()){
      //randomVerticalSeam();
      verticalSeam();
      //scImage->setWidth(currentWidth-1);       
    }
    currentWidth = width;
  }
  createImageToShow();
}

void SeamCarving::createImageToShow(){
  std::size_t xt = 0;
  imageToDisplay->setSameSize(currentWidth, scImage->getHeight());
  for(int y = 0; y < scImage->getHeight(); y++){
    for(int x = 0; x < scImage->getWidth(); x++){
      if(scImage->isSeam(x, y) && scImage->getSeanNum(x, y) <=
	 (scImage->getWidth() - currentWidth))
	imageEnergy->setPixelColor(x, y, Color(255,0,0));
      else{
	imageEnergy->setPixelColor(x, y, Color((*scImage)[y][x].getEnergy()/energy->MAX_ENERGY));
	imageToDisplay->setPixelColor(xt, y, scImage->getPixelColor(x, y));
	xt++;
      }
    }
  }
}


ScPixel::enUnit SeamCarving::computeEnergy(std::size_t x, std::size_t y){
  return energy->computePixelEnergy(x, y, *scImage);
}

ScPixel* SeamCarving::getPixelAbove(int x, int y){
  return (*scImage)[y][x].getNeighbour(ScPixel::UP);
    
  int nth = (int)(*scImage)[y-1][x].countSeams - (int)(*scImage)[y][x].countSeams;
  ScPixel *up = &(*scImage)[y-1][x];

  if(nth == 0 && up->nthSeam != 0)
    nth = -1;
  
  if(nth < 0){
    size_t a = x;
    for(; nth != 0; a--){
      if((*scImage)[y-1][a].nthSeam == 0)
	nth++;
    }
    up = &(*scImage)[y-1][a+1];
  }
  else if(nth > 0){
    int a = x;
    for(; nth != 0; a++){
      if((*scImage)[y-1][a].nthSeam == 0)
	nth--;
    }
    up = &(*scImage)[y-1][a-1];
  }
  return up;
}
void SeamCarving::setNextSeamPx(int xt, int x, int y){
  auto up = getPixelAbove(x, y);
  (*scImage)[y][x].dir = xt;
  switch(xt){
  case 0:
    (*scImage)[y][x].nextSeamPx = up;
    break;
  case 1:
    (*scImage)[y][x].nextSeamPx =
      up->getNeighbour(ScPixel::RIGHT);
    break;
  case -1:
    (*scImage)[y][x].nextSeamPx =
      up->getNeighbour(ScPixel::LEFT);
    break;
  }
}

void SeamCarving::verticalSeam(){

  std::shared_ptr<std::vector<ScPixel::enUnit>>
    totalEnergy =
    std::make_shared<std::vector<ScPixel::enUnit>>
    (std::vector<ScPixel::enUnit>(imageEnergy->getWidth()-seams.size())),

    totalEnergyTmp =
    std::make_shared<std::vector<ScPixel::enUnit>>
    (std::vector<ScPixel::enUnit>(imageEnergy->getWidth()-seams.size()));
  
  for(std::size_t x = 0, count = 0; x < imageEnergy->getWidth(); x++){
    if((*scImage)[0][x].nthSeam != 0)
      count++;
    (*scImage)[0][x].countSeams = count;
  }
  for(std::size_t x = 0; x < imageEnergy->getWidth(); x++){
    if((*scImage)[0][x].nthSeam == 0){
      (*totalEnergy)   [x-(*scImage)[0][x].countSeams] =
      (*totalEnergyTmp)[x-(*scImage)[0][x].countSeams] =
	(*scImage)[0][x].getEnergy();
      //std::cout << x-(*scImage)[0][x].countSeams << " " << (*scImage)[0][x].getEnergy() << "    " ;
    }
  }
  
  for(std::size_t y = 1; y < scImage->getHeight(); y++){
    int count = 0;
    
    for(std::size_t x = 0; x < imageEnergy->getWidth(); x++){
      if((*scImage)[y][x].nthSeam != 0){
	count++;
	(*scImage)[y][x].countSeams = count;
	continue;
      }
      (*scImage)[y][x].countSeams = count;
      //std::cout << "curEnergy: " << (*scImage)[y][x].getEnergy() << "\n";
      //for(auto &a: *totalEnergy)
      //std::cout << a << " ";
      
      if(x-count == 0){
	//std::cout << "first: " << x << std::endl;
	auto minElem = std::min_element((*totalEnergy).begin(), ((*totalEnergy).begin()+2));
	(*totalEnergyTmp)[0] = *minElem + (*scImage)[y][x].getEnergy();
	setNextSeamPx(minElem-(*totalEnergy).begin(), x, y);
	//std::cout << "minEnergy: " << *minElem << std::endl;
      }
      else if(x-count+1 == imageEnergy->getWidth() - seams.size()){
	//std::cout << "last: " << x << std::endl;
	auto minElem = std::min_element(((*totalEnergy).end()-2), (*totalEnergy).end());
	(*totalEnergyTmp).back() = *minElem + (*scImage)[y][x].getEnergy();
	setNextSeamPx(minElem-((*totalEnergy).end()-1), x, y);
	//std::cout << "minEnergy: " << *minElem << std::endl;
      }
      else{
	//std::cout << "energy: " << (*totalEnergy)[x-count] << std::endl;

	auto minElem = std::min_element((*totalEnergy).begin()+(x-count-1),
					(*totalEnergy).begin()+(x-count+2));

	(*totalEnergyTmp)[x-count] = (*minElem + (*scImage)[y][x].getEnergy()); 

	//std::cout << "minEnergy: " << *minElem << std::endl;
		
	int xt = minElem - ((*totalEnergy).begin() + (x-count));
	setNextSeamPx(xt, x, y);
      }
      //std::cout << "sumEnergy: " << (*totalEnergyTmp)[x-count] << std::endl << std::endl;

    }
    std::swap(totalEnergy, totalEnergyTmp);
  }

  //for(auto &a: *totalEnergy)
  //std::cout << a << " ";
  
  auto minElem = std::min_element((*totalEnergy).begin(), (*totalEnergy).end());
  int nth = minElem - (*totalEnergy).begin() +1;

  int a = 0;
  //if(nth == 0 && (*scImage)[scImage->getHeight()-1][a].nthSeam != 0){
  //nth += 1;
  //}
      
  //std::cout << "nth:  " << nth << "\n";
  for(; nth != 0; a++){
    if((*scImage)[scImage->getHeight()-1][a].nthSeam == 0)
      nth--;
  }
  //std::cout << "index:  " <<  a << "\n";

  ScPixel *px = &(*scImage)[scImage->getHeight()-1][a-1]; //min total energy
  seams.push_back(px);

  for(int i = 0, dir; px != nullptr; i++, dir = px->dir, px = px->nextSeamPx){
    if(px->nthSeam != 0)
      std::cout << "error   " << i << "\n";

    px->nthSeam = seams.size();

    if(px->getNeighbour(ScPixel::LEFT) != nullptr){
      px->getNeighbour(ScPixel::LEFT)-> setNeighbour(px->getNeighbour(ScPixel::RIGHT), ScPixel::RIGHT);

      if(px->getNeighbour(ScPixel::LEFT)->nthSeam != 0)
	std::cout << "error left\n";
    }
    if(px->getNeighbour(ScPixel::RIGHT) != nullptr){
     px->getNeighbour(ScPixel::RIGHT)->setNeighbour(px->getNeighbour(ScPixel::LEFT),  ScPixel::LEFT);

     if(px->getNeighbour(ScPixel::RIGHT)->nthSeam != 0)
       std::cout << "error right\n";
    }
    
#if 1
    if(px->getNeighbour(ScPixel::DOWN) != nullptr && px->getNeighbour(ScPixel::DOWN)->nthSeam == 0){
      auto ngbDown = px->getNeighbour(ScPixel::DOWN);
      auto ngbUp = dir == 1?
	px->getNeighbour(ScPixel::LEFT):
	px->getNeighbour(ScPixel::RIGHT);
           
      ngbDown->setNeighbour(ngbUp, ScPixel::UP);

      if(ngbUp == nullptr)
	std::cout << "here you are " << dir << std::flush;

      ngbUp->setNeighbour(ngbDown, ScPixel::DOWN);
      
      if(px->getNeighbour(ScPixel::DOWN)->nthSeam != 0)
	std::cout << "error down\n";
    }
#endif
  }
}

