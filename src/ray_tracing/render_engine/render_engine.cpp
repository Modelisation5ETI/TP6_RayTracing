/*
**    TP CPE Lyon
**    Copyright (C) 2015 Damien Rohmer
**
**    This program is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**   This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "render_engine.hpp"

#include "image/image.hpp"
#include "ray_tracing/scene/scene_parameter.hpp"
#include "ray_tracing/scene/ray.hpp"
#include "ray_tracing/primitives/intersection_data.hpp"
#include "ray_tracing/scene/anti_aliasing_table.hpp"

#include <cmath>

namespace cpe
{


void render(image& im,scene_parameter const& scene)
{
  // **************************************************************************** //
  // For all pixels of the image
  //    Generate ray from the camera toward in the direction of the current pixel
  //    Compute associated color (ray tracing algorithm)
  //    Set the color to the current pixel
  //
  // **************************************************************************** //

  camera const& cam = scene.get_camera();

  int const Nx = im.Nx();
  int const Ny = im.Ny();

  // loop over all the pixels of the image
  for(int kx=0 ; kx<Nx ; ++kx)
    {
    float const u = static_cast<float>(kx)/(Nx-1);
    for(int ky=0 ; ky<Ny ; ++ky)
      {
      float const v = static_cast<float>(ky)/(Ny-1);

      // Anti-aliasing
      int const N_sample = 5;
      anti_aliasing_table aa( N_sample );
      color c ;

      for(int dx=0 ; dx<N_sample ; ++dx)
        {
        for(int dy=0 ; dy<N_sample ; ++dy)
          {
          float const du = aa.displacement(dx)/(Nx-1); //Nx is the size in pixel in x direction
          float const dv = aa.displacement(dy)/(Ny-1); //Ny is the size in pixel in y direction
          float const w = aa.weight(dx,dy); //Weight

          // Generate displaced ray and compute color
          ray const r = ray_generator(cam,u + du,v + dv);
          c = c + w * ray_trace(r,scene,3);
          }
        }
      im({kx,ky}) = c;
      }
    }
}


ray ray_generator(camera const& cam,float const u,float const v)
{
    // position of the sample on the screen in 3D
    vec3 const p_screen = screen_position(cam,u,v);

    // vector "camera center" to "screen position"
    vec3 const d = p_screen-cam.center();

    // compute the ray
    ray const r(cam.center(),d);

    return r;
}

color ray_trace(ray const& r,scene_parameter const& scene, int nb_reflection)
{
  intersection_data intersection; //current intersection
  int intersected_primitive = 0;  //current index of intersected primitive

  bool const is_intersection = compute_intersection(r,scene,intersection,intersected_primitive);
  if(is_intersection) //if the ray intersects a primitive
    {
    material const& mat = scene.get_material(intersected_primitive);
    color colorReflection =  compute_illumination( mat,intersection,scene );
    // Reflexions
    ray ray_reflected = ray( intersection.position + 0.0001 * intersection.normal , reflected(-r.u(), intersection.normal) );
    if(nb_reflection > 0)
      {
      colorReflection += 0.2 * ray_trace(ray_reflected,scene,nb_reflection-1);  //attenuation 0.2
      }
    return colorReflection;
    }
  else
    {
    return color(0,0,0); //no intersection
    }
}


bool compute_intersection(ray const& r,
                          scene_parameter const& scene,
                          intersection_data& intersection,
                          int& index_intersected_primitive)
{
  int const N_primitive = scene.size_primitive();

  bool found_intersection = false;
  float minDist = 1e20;
  intersection_data intersection_tmp;

  int k = 0;
  // Iterate through all primitives
  while( k < N_primitive )
    {
    primitive_basic const & primitive = scene.get_primitive( k );
    // Compute intersection
    bool is_intersection = primitive.intersect( r, intersection_tmp );
    // If intersection if closer
    if( is_intersection && intersection_tmp.relative < minDist)
      {
      // Update temporary intersection and minimum distance
      found_intersection = true;
      minDist = intersection_tmp.relative;
      index_intersected_primitive = k;
      intersection = intersection_tmp;
      }
    ++k;
    }
  return found_intersection;
}


bool is_in_shadow(vec3 const& p,vec3 const& p_light, scene_parameter const& scene)
{
  int const N_primitive = scene.size_primitive();

  intersection_data intersection;
  ray r = ray( p, normalized(p_light - p) );
  r.offset(); //Offset the ray to prevent intersection of pixels on edges with their primitives

  int k = 0;
  // Iterate through all primitives
  while( k < N_primitive )
    {
    primitive_basic const & primitive = scene.get_primitive( k );
    // Check intersection between pixel and light
    if( primitive.intersect( r, intersection ) )
      {
      if( intersection.relative <= norm(p_light - p) )
        {
        return true;
        }
      }
    ++k;
    }

  return false;
}



color compute_illumination(material const& mat,intersection_data const& intersection,scene_parameter const& scene)
{
  // ********************************************* //
  //   Pour toutes les lumieres
  //     Si point dans l'ombre
  //       Ajouter a couleur courante une faible part de la couleur du materiau
  //          ex. c += mat.color_object()/10.0f;
  //     Sinon
  //       Calculer illumination au point courant en fonction de la position
  //          de la lumiere et de la camera
  //       Ajouter a couleur courante la contribution suivante:
  //           puissance de la lumiere (L.power)
  //         X couleur de la lumiere (L.c)
  //         X valeur de l'illumination
  //
  // ********************************************* //

  color c;

  vec3 const& p0 = intersection.position;

  int const N_light = scene.size_light();
  for(int k=0; k<N_light ; ++k)
    {
    light const& L = scene.get_light(k);
    bool const shadow = is_in_shadow(p0,L.p,scene);
    if(shadow)
      c += mat.color_object()/10.0f;
    else
      {
      color c_shading = compute_shading( mat.shading(), mat.color_object(), p0, L.p, intersection.normal, scene.get_camera().center() );
      c += L.power * L.c * c_shading / ( norm(p0- L.p) * norm(p0- L.p) );
      }
    }

  return c;
}





}


