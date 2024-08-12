#include <QDebug>
#include <editor/mml/MmlHighlighter.hpp>
#include <sft/mml/MmlParser.hpp>

using namespace Editor;

// ----------------------------------------------------------------------------
MmlHighlighter::MmlHighlighter(QTextDocument *parent)
  : QSyntaxHighlighter(parent)
  , mParser(new Sft::MmlParser(this))
{
  mErrorFormat.setUnderlineColor(QColor(255, 0, 0));
  mErrorFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);

  mCommentFormat.setForeground(QBrush(QColor(164, 164, 164)));
  mNoteFormat.setForeground(QBrush(QColor(0, 255, 128)));
  mInstrumentFormat.setForeground(QBrush(QColor(128, 255, 255)));
}

// ----------------------------------------------------------------------------
MmlHighlighter::~MmlHighlighter() {
  delete mParser;
}

// ----------------------------------------------------------------------------
void MmlHighlighter::highlightBlock(const QString &text) {
  mParser->loadFromString(text);
}

// ----------------------------------------------------------------------------
void MmlHighlighter::error(int, int, const QString &) {
  setFormat(info().start, info().length(), mErrorFormat);
}

// ----------------------------------------------------------------------------
void MmlHighlighter::comment() {
  setFormat(info().start, info().length(), mCommentFormat);
}

// ----------------------------------------------------------------------------
void MmlHighlighter::setAttribute(const QString &, const QString &) {
}

// ----------------------------------------------------------------------------
void MmlHighlighter::beginTrack(int) {
}

// ----------------------------------------------------------------------------
void MmlHighlighter::note(QChar, bool, int) {
  setFormat(info().start, info().length(), mNoteFormat);
}

// ----------------------------------------------------------------------------
void MmlHighlighter::decrementOctave() {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::incrementOctave() {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::setTrackRepeatOffset() {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::startLoop() {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::endLoop(int) {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::resumeNote(int) {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::pause(int) {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::setInstrument(const QString &) {
  setFormat(info().start, info().length(), mInstrumentFormat);
}

// ----------------------------------------------------------------------------
void MmlHighlighter::setOctave(int) {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::setTempo(int) {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::setVolume(int) {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::setPanning(int) {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::setEchoEnabled(bool) {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::setPitchChannelEnabled(bool) {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::easePitch(uint8_t, int) {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::vibratePitch(uint8_t, float) {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::setPitchOffset(float, bool) {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::setAdsrA(int) {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::setAdsrD(int) {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::setAdsrS(int) {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::setAdsrR(int) {

}

// ----------------------------------------------------------------------------
void MmlHighlighter::setNoiseEnabled(bool, int) {

}