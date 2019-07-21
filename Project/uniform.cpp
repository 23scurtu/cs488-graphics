 #include "uniform.hpp"

std::default_random_engine generator;
std::uniform_real_distribution<float> uni_dist(0.0f, 1.0f);
 
float uni()
{
    return uni_dist(generator);
}