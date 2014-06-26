/*
 *  KSeg
 *  Copyright (C) 1999-2006 Ilya Baran
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Send comments and/or bug reports to:
 *                 ibaran@mit.edu
 */


#ifndef G_MATRIX_H
#define G_MATRIX_H

#include "G_point.H"
#include "G_segment.H"
#include "G_ref.H"
#include "my_hash_map.H"

//a 2x3 matrix representing a homogeneous 2d transform
//identity =
// 1 0 0
// 0 1 0

class G_matrix
{
private:
  double v[2][3];

public:

  G_matrix() { v[0][0] = v[1][1] = 1; v[0][1] = v[1][0] = v[0][2] = v[1][2] = 0; }
  G_matrix(const G_matrix &m) { v[0][0] = m.v[0][0]; v[1][0] = m.v[1][0]; v[0][1] = m.v[0][1];
                                v[1][1] = m.v[1][1]; v[0][2] = m.v[0][2]; v[1][2] = m.v[1][2]; }

  const G_matrix &operator=(const G_matrix &m)  { v[0][0] = m.v[0][0]; v[1][0] = m.v[1][0]; v[0][1] = m.v[0][1];
                         v[1][1] = m.v[1][1]; v[0][2] = m.v[0][2]; v[1][2] = m.v[1][2]; return *this;}

  G_matrix operator*(const G_matrix &m) const {
    G_matrix out;
    out.v[0][0] = v[0][0] * m.v[0][0] + v[0][1] * m.v[1][0];
    out.v[0][1] = v[0][0] * m.v[0][1] + v[0][1] * m.v[1][1];
    out.v[1][0] = v[1][0] * m.v[0][0] + v[1][1] * m.v[1][0];
    out.v[1][1] = v[1][0] * m.v[0][1] + v[1][1] * m.v[1][1];
    out.v[0][2] = v[0][0] * m.v[0][2] + v[0][1] * m.v[1][2] + v[0][2];
    out.v[1][2] = v[1][0] * m.v[0][2] + v[1][1] * m.v[1][2] + v[1][2];
    return out;
  }

  G_point operator*(const G_point &p) const { return G_point(p.getX() * v[0][0] + p.getY() * v[0][1] + v[0][2],
						       p.getX() * v[1][0] + p.getY() * v[1][1] + v[1][2]); }
  
  bool operator==(const G_matrix &m) const { return v[0][0] == m.v[0][0] && v[1][0] == m.v[1][0] &&
					       v[0][1] == m.v[0][1] && v[1][1] == m.v[1][1] &&
					       v[0][2] == m.v[0][2] && v[1][2] == m.v[1][2]; }


  bool almostEqual(const G_matrix &m) const {
    double d;
    d = fabs(v[0][0] - m.v[0][0]) + fabs(v[0][1] - m.v[0][1]) + fabs(v[0][2] - m.v[0][2]);
    d += fabs(v[1][0] - m.v[1][0]) + fabs(v[1][1] - m.v[1][1]) + fabs(v[1][2] - m.v[1][2]);
    return d < 0.000001;
  }

  static G_matrix scale(double s) { G_matrix m; m.v[0][0] = m.v[1][1] = s; return m; }
  static G_matrix rotate(double a) 
  { G_matrix m; m.v[0][0] = cos(a); m.v[0][1] = sin(a); m.v[1][0] = -sin(a); m.v[1][1] = cos(a); return m; }
  static G_matrix translate(double x, double y) { G_matrix m; m.v[0][2] = x; m.v[1][2] = y; return m; }
  static G_matrix reflect(double x, double y)
  { G_matrix m; G_point p(1, 0); p.reflect(G_segment(G_point(0, 0), G_point(x, y))); m.v[0][0] = p.getX();
    m.v[1][0] = p.getY();
    p = G_point(0, 1); p.reflect(G_segment(G_point(0, 0), G_point(x, y)));
    m.v[0][1] = p.getX(); m.v[1][1] = p.getY(); return m; }

  static G_matrix getNullMatrix()
  { G_matrix m; m.v[0][0] = m.v[0][1] = m.v[0][2] = m.v[1][0] = m.v[1][1] = m.v[1][2] = BIG; return m;}

  void printMatrix()
  {
    printf("%f %f %f\n", v[0][0], v[0][1], v[0][2]);
    printf("%f %f %f\n", v[1][0], v[1][1], v[1][2]);
  }
};

hash_map<G_ref *, G_matrix> BuildReverse(const G_refs &refs);


#endif //G_MATRIX_H
