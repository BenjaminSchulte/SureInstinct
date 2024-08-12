#pragma once

#include <QSharedPointer>
#include "../render/Renderable.hpp"

class QPainter;

namespace Sft {
class Note;
class Channel;
typedef QSharedPointer<class Sequence> SequencePtr;
struct MmlExecutedCommand;
}

namespace Editor {
class ProjectContext;
class NoteEditorTool;

class PianoRollNoteEditor : public Renderable {
public:
  //! Location info
  struct LocationInfo {
    //! Constructor
    LocationInfo() : mSequence(nullptr), mSequenceOffset(0), mTick(0), mSubTick(0), mNote(0) {}

    //! Constructor
    LocationInfo(const Sft::SequencePtr &sequence, uint32_t sequenceOffset, uint32_t tick, uint16_t subTick, uint16_t note) : mSequence(sequence), mSequenceOffset(sequenceOffset), mTick(tick), mSubTick(subTick), mNote(note) {}

    //! Returns the sequence
    inline const Sft::SequencePtr &sequence() const { return mSequence; }

    //! Returns the tick
    inline uint32_t relativeTick() const { return mTick; }

    //! Returns the subtick
    inline uint16_t subTick() const { return mSubTick; }

    //! Returns the note
    inline uint16_t note() const { return mNote; }

    //! Returns the absolute tick
    inline uint32_t absoluteTick() const { return mSequenceOffset + mTick; }

    //! Returns the absolute tick
    inline uint32_t sequenceOffset() const { return mSequenceOffset; }

  protected:
    //! Sequence pointer
    Sft::SequencePtr mSequence;

    //! The sequence offset
    uint32_t mSequenceOffset;

    //! The tick
    uint32_t mTick;

    //! The sub tick
    uint16_t mSubTick;

    //! The note
    uint16_t mNote;
  };

  //! Constructor
  PianoRollNoteEditor(RenderWidget *parent, const QRect &pos) : Renderable(parent, pos) {}

  //! Deconstructor
  ~PianoRollNoteEditor();

  //! Sets the context
  inline void setProject(ProjectContext *context) { mContext = context; }

  //! Sets the current channel
  inline void setChannel(Sft::Channel *channel) { mChannel = channel; }

  //! Sets the margin left
  inline void setScroll(int x, int y) { mScrollLeft = x; mScrollTop = y; }

  //! Sets the zoom
  inline void setZoom(float zoom) { mZoom = zoom; }

  //! Draws this renderable
  void draw(RenderHelper &) override;

  //! Sets the current editor tool
  void setCurrentEditTool(NoteEditorTool *tool);

  //! Sets the current editor tool and clicks
  bool setCurrentEditToolAndClick(NoteEditorTool *tool, Qt::MouseButton button, const QPoint &p);

  //! Returns the project context
  inline ProjectContext *context() const { return mContext; }

protected:
  //! The mouse move event
  bool mousePressEvent(Qt::MouseButton, const QPoint &) override;

  //! The mouse move event
  bool mouseMoveEvent(const QPoint &) override;

  //! The mouse move event
  bool mouseReleaseEvent(Qt::MouseButton, const QPoint &) override;

  //! Zoom event
  bool zoomEvent(float) override;

  //! Draws the rows
  void drawRows(RenderHelper &h, QPainter *p, int topNote, int sy, int pkh, int bottom);

  //! Draws the rows
  void drawCols(RenderHelper &h, QPainter *p, int leftTick, int sx, int mtw);

  //! Draws the rows
  void drawNotes(RenderHelper &h, QPainter *p, Sft::Channel *channel, int leftTick, int topNote, int mtw, int sx, int sy, bool alpha=false);

  //! Draws the rows
  void drawNote(RenderHelper &h, QPainter *p, const Sft::MmlExecutedCommand &, int leftTick, int topNote, int sx, int sy, bool alpha=false);

  //! Draws the rows
  void drawCommand(RenderHelper &h, QPainter *p, const QString &, int leftTick, int sx);

  //! The project context
  ProjectContext *mContext = nullptr;

  //! The channel
  Sft::Channel *mChannel = nullptr;

  //! The current edit tool
  NoteEditorTool *mCurrentEditTool = nullptr;

  //! The left scroll
  int mScrollLeft = 0;

  //! The top scroll
  int mScrollTop = 0;

  //! The measure tick width
  int mMeasureTickWidth = 1;

  //! The key height
  int mKeyHeight = 1;

  //! Current zoom
  float mZoom = 1.0f;

  //! Whether editor window update event is blocked
  bool mBlockEditorWindowUpdate = false;
};

}