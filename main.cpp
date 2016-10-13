#ifdef _WIN32
#define M_PI 3.1415926535897932384626433832795
static long long  x=0x1234ABCD330E;

double drand48() /* 0.0ˆÈã1.0–¢–ž */
{
    x = x*0x5DEECE66D + 0xB;
    return (x&0xFFFFFFFFFFFF)*(1.0/281474976710656.0);
}

long lrand48() /* 0ˆÈã2147483647ˆÈ‰º */
{
    x = x*0x5DEECE66D + 0xB;
    return(long)(x>>17)&0x7FFFFFFF;
}

long mrand48() /* -2147483648ˆÈã214748367ˆÈ‰º */
{
    x = x*0x5DEECE66D + 0xB;
    return(long)(x>>16)&0xFFFFFFFF;
}

void srand48(long s)
{
    x = s;
    x = (x<<16)+0x330E;
}
#endif


#include <iostream>
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "float.h"
#include "material.h"

vec3 color(const ray& r, hitable *world, int depth) {
  hit_record rec;
  if(world->hit(r, 0.001, FLT_MAX, rec)){
    ray scattered;
    vec3 attenuation;
    if(depth < 50 && rec.mat_ptr -> scatter(r, rec, attenuation, scattered)){
      return attenuation * color(scattered, world, depth+1);
    }else{
      return vec3(0, 0, 0);
    }
  }
  else {
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0-t) * vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
  }
}

int main() {
  int nx = 400;
  int ny = 200;
  int ns = 100;
  std::cout << "P3\n" << nx << " " << ny << "\n255\n";

  float R = cos(M_PI / 4);

  const int num = 500;
  hitable *list[num];
  list[0] = new sphere(vec3(0, -1000, -1), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
  int i = 1;
  for(int a = -11; a < 11; a++){
    for(int b = -11; b < 11; b++){
      float choose_mat = drand48();
      vec3 center(a + 0.9 * drand48(), 0.2, b + 0.9 * drand48());
      if((center - vec3(4, 0.2, 0)).length() > 0.9){
        if(choose_mat < 0.8){ // diffuse
          list[i++] = new sphere(center, 0.2, new lambertian(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48())));
        }else if(choose_mat < 0.95){  // metal
          list[i++] = new sphere(center, 0.2, new metal(vec3(0.5 * (1 + drand48()), 0.5 * (1 + drand48()), 0.5 * (1 + drand48())), 0.5 * (1 + drand48())));
        }else{  // glass
          list[i++] = new sphere(center, 0.2, new dielectric(1.5));
        }
      }
    }
  }
  list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
  list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
  list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

  hitable *world = new hitable_list(list, i);
  int tnum = i;

  vec3 lookfrom(8, 2, 5);
  vec3 lookat(0, 1, 0);
  float dist_to_focus = 10.0; // (lookfrom - lookat).length();
  float aperture = 0.1;

  // camera  lookfrom, lookat, vup
  camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx)/float(ny), aperture, dist_to_focus);

  for(int j = ny - 1; j >= 0; j--){
    for(int i = 0; i < nx; i++){
      vec3 col(0, 0, 0);
      for(int s = 0; s < ns; s++){
        float u = float(i + drand48()) / float(nx);
        float v = float(j + drand48()) / float(ny);
        ray r = cam.get_ray(u, v);
        vec3 p = r.point_at_parameter(2.0);
        col += color(r, world, 0);
      }
      col /= float(ns);
      col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
      int ir = int(255.99 * col[0]);
      int ig = int(255.99 * col[1]);
      int ib = int(255.99 * col[2]);

      std::cout << ir << " " << ig << " " << ib << "\n";
    }
  }
  delete world;
  for(int i = 0; i < tnum; i++) delete list[i];
}

