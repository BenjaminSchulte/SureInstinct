#include <editor/project/ProjectContext.hpp>
#include <sft/song/Song.hpp>

using namespace Editor;

// ----------------------------------------------------------------------------
void ProjectContext::setActiveInstrument(const Sft::InstrumentPtr &instrument) {
  if (instrument == mInstrument) {
    return;
  }

  mInstrument = instrument;
  emit activeInstrumentChanged(instrument);
}

// ----------------------------------------------------------------------------
void ProjectContext::setActiveChannel(Sft::Channel *channel) {
  if (channel == mChannel) {
    return;
  }

  mChannel = channel;
  emit activeChannelChanged(channel);
}

// ----------------------------------------------------------------------------
void ProjectContext::notifyCodeChanged() {
  emit codeChanged();
}

// ----------------------------------------------------------------------------
void ProjectContext::setEditorScroll(float leftNote) {
  if (leftNote == mEditorWindowLeft) {
    return;
  }

  mEditorWindowLeft = leftNote;
  emit editorScrollChanged(leftNote);
}

// ----------------------------------------------------------------------------
void ProjectContext::setEditorWidth(float numNotes) {
  if (numNotes == mEditorWindowWidth) {
    return;
  }

  mEditorWindowWidth = numNotes;
  emit editorWidthChanged(numNotes);
}
