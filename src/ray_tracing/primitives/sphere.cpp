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

#include "sphere.hpp"

#include "intersection_data.hpp"
#include "../scene/ray.hpp"
#include "lib/common/error_handling.hpp"

#include <cmath>

namespace cpe
{

sphere::sphere(vec3 const& center_param,float radius_param)
    :center_data(center_param),radius_data(radius_param)
{}

vec3 const& sphere::center() const
{
    return center_data;
}
float sphere::radius() const
{
    return radius_data;
}

bool sphere::intersect(ray const& ray_param,intersection_data& intersection) const
{

    vec3 const& u = ray_param.u();

    // ********************************************************** //
    // ********************************************************** //
    //  TO DO:
    //    Calcul d'intersection entre un rayon et une plan
    //
    // Variales:
    //  - Position initiale du rayon: ray_param.p0()
    //  - Vecteur directeur unitaire du rayon: u
    //  - Position du centre de la sphere: center_data
    //  - Rayon de la sphere: radius_data
    //
    // Aide de syntaxe:
    //  - Norme au carre d'un vecteur v: float norme=v.norm2();
    //             ou: float norme=v.dot(v);
    //
    // ********************************************************** //
    // ********************************************************** //

    vec3 const& xs = ray_param.p0();
    vec3 const& x0 = center_data;

    float t_inter;

    //Résolution système 2nd ordre en t_inter
    float a = dot( u,u );
    float b = 2 * dot( xs - x0, u );
    float c = dot( xs - x0, xs - x0 ) - radius_data * radius_data;

    float Delta = b * b - 4.0f * a * c;
    if( Delta < 0.0 )
      return false;

    if( Delta > 0.0 )
      {
      float const t_inter1 = ( -b - sqrt(Delta) )/( 2.0f*a );
      float const t_inter2 = ( -b + sqrt(Delta) )/( 2.0f*a );
      if(t_inter1 < 0.0 && t_inter2 < 0.0 )
         return false;
      if( t_inter1 < 0.0 || t_inter2 < 0.0 )
        t_inter = std::max( t_inter1, t_inter2 );
      else
        t_inter = std::min( t_inter1, t_inter2 );
      vec3 const p_inter = xs + t_inter * u;
      vec3 const n_inter = normalized( p_inter - x0 );
      intersection.set( p_inter, n_inter, t_inter );
      return true;
      }

    if( Delta < 1e-8 || Delta > 1e-8)
      {
      t_inter = -b/(2.0f*a);
      vec3 const p_inter = xs + t_inter * u;
      vec3 const n_inter = normalized( p_inter - x0 );
      intersection.set( p_inter, n_inter, t_inter );
      return true;
      }

    return false;
}



}
