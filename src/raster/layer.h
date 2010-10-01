/* ASE - Allegro Sprite Editor
 * Copyright (C) 2001-2010  David Capello
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef RASTER_LAYER_H_INCLUDED
#define RASTER_LAYER_H_INCLUDED

#include <string>
#include "raster/gfxobj.h"

class Cel;
class Image;
class Sprite;
class Layer;
class LayerImage;
class LayerFolder;

#define LAYER_IS_READABLE	0x0001
#define LAYER_IS_WRITABLE	0x0002
#define LAYER_IS_LOCKMOVE	0x0004
#define LAYER_IS_BACKGROUND	0x0008

//////////////////////////////////////////////////////////////////////
// Layer class

class Layer : public GfxObj
{
protected:
  Layer(GfxObjType type, Sprite* sprite);
  Layer(Sprite* sprite);
  Layer(const Layer* src_layer, Sprite* dst_sprite);

public:
  virtual ~Layer();

  std::string getName() const { return m_name; }
  void setName(const std::string& name) { m_name = name; }

  Sprite* getSprite() const { return m_sprite; }
  LayerFolder* get_parent() const { return m_parent; }
  void set_parent(LayerFolder* folder) { m_parent = folder; }
  Layer* get_prev() const;
  Layer* get_next() const;

  bool is_image() const { return getType() == GFXOBJ_LAYER_IMAGE; }
  bool is_folder() const { return getType() == GFXOBJ_LAYER_FOLDER; }
  bool is_background() const { return (m_flags & LAYER_IS_BACKGROUND) == LAYER_IS_BACKGROUND; }
  bool is_moveable() const { return (m_flags & LAYER_IS_LOCKMOVE) == 0; }
  bool is_readable() const { return (m_flags & LAYER_IS_READABLE) == LAYER_IS_READABLE; }
  bool is_writable() const { return (m_flags & LAYER_IS_WRITABLE) == LAYER_IS_WRITABLE; }

  void set_background(bool b) { if (b) m_flags |= LAYER_IS_BACKGROUND; else m_flags &= ~LAYER_IS_BACKGROUND; }
  void set_moveable(bool b) { if (b) m_flags &= ~LAYER_IS_LOCKMOVE; else m_flags |= LAYER_IS_LOCKMOVE; }
  void set_readable(bool b) { if (b) m_flags |= LAYER_IS_READABLE; else m_flags &= ~LAYER_IS_READABLE; }
  void set_writable(bool b) { if (b) m_flags |= LAYER_IS_WRITABLE; else m_flags &= ~LAYER_IS_WRITABLE; }

  virtual void getCels(CelList& cels) = 0;
  virtual Layer* duplicate_for(Sprite* sprite) const = 0;

  // TODO remove these methods (from C backward-compatibility)
  unsigned short* flags_addr() { return &m_flags; }

private:
  std::string m_name;		// layer name
  Sprite* m_sprite;		// owner of the layer
  LayerFolder* m_parent;	// parent layer
  unsigned short m_flags;
};

//////////////////////////////////////////////////////////////////////
// LayerImage class

class LayerImage : public Layer
{
public:
  LayerImage(Sprite* sprite);
  LayerImage(const LayerImage* copy, Sprite* sprite);
  virtual ~LayerImage();

  int get_blend_mode() const { return m_blend_mode; }
  void set_blend_mode(int blend_mode);

  void addCel(Cel *cel);
  void removeCel(Cel *cel);
  const Cel* getCel(int frame) const;
  Cel* getCel(int frame);

  void getCels(CelList& cels);

  void configureAsBackground();

  CelIterator getCelBegin() { return m_cels.begin(); }
  CelIterator getCelEnd() { return m_cels.end(); }
  CelConstIterator getCelBegin() const { return m_cels.begin(); }
  CelConstIterator getCelEnd() const { return m_cels.end(); }
  int getCelsCount() const { return m_cels.size(); }

  LayerImage* duplicate_for(Sprite* sprite) const { return new LayerImage(this, sprite); }

private:
  void destroy_all_cels();

  int m_blend_mode; // It indicates how this layer is merged in the final render.
  CelList m_cels;   // List of all cels inside this layer used by frames.
};

//////////////////////////////////////////////////////////////////////
// LayerFolder class

class LayerFolder : public Layer
{
public:
  LayerFolder(Sprite* sprite);
  LayerFolder(const LayerFolder* copy, Sprite* sprite);
  virtual ~LayerFolder();

  LayerList get_layers_list() { return m_layers; }
  LayerIterator get_layer_begin() { return m_layers.begin(); }
  LayerIterator get_layer_end() { return m_layers.end(); }
  LayerConstIterator get_layer_begin() const { return m_layers.begin(); }
  LayerConstIterator get_layer_end() const { return m_layers.end(); }
  int get_layers_count() const { return m_layers.size(); }

  void add_layer(Layer* layer);
  void remove_layer(Layer* layer);
  void move_layer(Layer* layer, Layer* after);

  void getCels(CelList& cels);

  LayerFolder* duplicate_for(Sprite* sprite) const { return new LayerFolder(this, sprite); }

private:
  void destroyAllLayers();

  LayerList m_layers;
};

Layer* layer_new_flatten_copy(Sprite* dst_sprite, const Layer* src_layer,
			      int x, int y, int w, int h, int frmin, int frmax);

void layer_render(const Layer* layer, Image *image, int x, int y, int frame);

#endif
