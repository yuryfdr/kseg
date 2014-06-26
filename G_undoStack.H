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


#ifndef G_UNDOSTACK_H
#define G_UNDOSTACK_H

#include <QStack>
#include "G_undo.H"

class G_undoStack : public QStack<QSharedPointer<G_undo> >
{
public:
  G_undoStack(KSegDocument *inDoc) { doc = inDoc; }

  void DoIt();
  void opFinished() { if(!isEmpty()) top()->setLastOne(true); }

protected:
  
private:
  KSegDocument *doc;
};

#endif //G_UNDOSTACK_H
