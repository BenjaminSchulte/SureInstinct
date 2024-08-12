#pragma once

#include <QDir>
#include <QString>
#include <QVector>

namespace Maker {

struct GameVersionString {
  //! Constructor
  GameVersionString() {}

  //! Constructor
  GameVersionString(const QString &version) : mVersion(version) {}

  //! Returns the version string
  inline const QString &toString() const { return mVersion; }

  //! Returns whether the version is valid
  inline bool isValid() const { return !mVersion.isEmpty(); }

private:
  //! The version string
  QString mVersion;
};

struct GameVersionCompareString {
  //! Constructor
  GameVersionCompareString() {}

  //! Constructor
  GameVersionCompareString(const QString &version) : mVersion(version) {}

  //! Returns the version string
  inline const QString &toString() const { return mVersion; }

  //! Does this version match
  bool matches(const GameVersionString &) const;

private:
  //! The version string
  QString mVersion;
};

struct GameModuleGameScriptRequirement {
  //! Constructor
  GameModuleGameScriptRequirement() {}

  //! Constructor
  GameModuleGameScriptRequirement(const QString &id, const QString &name, const QString &description)
    : mId(id)
    , mName(name)
    , mDescription(description)
  {}

  //! Returns the id string
  inline const QString &id() const { return mId; }

  //! Returns the name string
  inline const QString &name() const { return mName; }

  //! Returns the description string
  inline const QString &description() const { return mDescription; }

private:
  //! The ID
  QString mId;

  //! The name
  QString mName;

  //! The description
  QString mDescription;
};

struct GameModuleDependency {
  //! Constructor
  GameModuleDependency() {}

  //! Constructor
  GameModuleDependency(const QString &id, const GameVersionCompareString &version)
    : mId(id)
    , mVersion(version)
  {}

  //! Returns the id string
  inline const QString &id() const { return mId; }

  //! Returns the version string
  inline const GameVersionCompareString &version() const { return mVersion; }

private:
  //! The name
  QString mId;

  //! The version string
  GameVersionCompareString mVersion;
};

struct GameModuleProvide {
  //! Constructor
  GameModuleProvide() {}

  //! Constructor
  GameModuleProvide(const QString &id, const GameVersionString &version)
    : mId(id)
    , mVersion(version)
  {}

  //! Returns the id string
  inline const QString &id() const { return mId; }

  //! Returns the version string
  inline const GameVersionString &version() const { return mVersion; }

private:
  //! The name
  QString mId;

  //! The version string
  GameVersionString mVersion;
};


class GameModuleHeader {
public:
  //! Constructor
  GameModuleHeader(const QDir &path) : mPath(path) {}

  //! Returns the ID with the version attached to it
  inline QString fullId() const { return id() + "@" + version().toString(); }

  //! Loads the header
  bool load();

  //! Returns the id string
  inline const QDir &path() const { return mPath; }

  //! Returns the name string
  inline const QString &name() const { return mName; }

  //! Returns the id string
  inline const QString &id() const { return mId; }

  //! Returns the version string
  inline const GameVersionString &version() const { return mVersion; }

  //! Returns the id string
  inline const QStringList &authors() const { return mAuthors; }

  //! Returns the id string
  inline const QStringList &includes() const { return mIncludes; }

  //! Returns all includes with absolute path
  QStringList asmIncludePath() const;

  //! Returns the id string
  inline const QVector<GameModuleDependency> &dependencies() const { return mDependencies; }

  //! Returns the id string
  inline const QVector<GameModuleDependency> &interfaceDependencies() const { return mInterfaceDependencies; }

  //! Returns the id string
  inline const QVector<GameModuleProvide> &interfaceProviders() const { return mProvideInterfaces; }

private:
  //! The path of this module
  QDir mPath;

  //! The name
  QString mId;

  //! The name
  QString mName;

  //! The version
  GameVersionString mVersion;

  //! Author list
  QStringList mAuthors;

  //! Dependencies
  QVector<GameModuleDependency> mDependencies;

  //! Dependend interfaces
  QVector<GameModuleDependency> mInterfaceDependencies;

  //! List of interfaces
  QVector<GameModuleProvide> mProvideInterfaces;

  //! Include list
  QStringList mIncludes;
};

}
