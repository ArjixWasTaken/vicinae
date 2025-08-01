#pragma once
#include "argument.hpp"
#include "command.hpp"
#include "omni-icon.hpp"
#include "preference.hpp"
#include "theme.hpp"
#include <QKeyEvent>
#include <QString>
#include <memory>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qdir.h>
#include <qicon.h>
#include <qlabel.h>
#include <qlist.h>
#include <qlogging.h>
#include <qwidget.h>
#include "common.hpp"
#include "vicinae.hpp"

class AppWindow;
class ViewCommandContext;

class BuiltinCommand : public AbstractCmd {
  std::optional<OmniIconUrl> _url;
  QString _repositoryId;
  QString _repositoryName;

public:
  QString uniqueId() const override final { return _repositoryId + "." + id(); }

  virtual QString id() const = 0;
  QString name() const override = 0;
  QString description() const override { return ""; }
  OmniIconUrl iconUrl() const override = 0;
  virtual CommandType type() const override final { return CommandTypeBuiltin; }
  QString extensionId() const override { return _repositoryId; }
  QString commandId() const override { return id(); }

  QString author() const override { return Omnicast::APP_ID; }

  // TODO: remove
  void setRepositoryIconUrl(const OmniIconUrl &icon) {}

  void setRepositoryId(const QString &id) { _repositoryId = id; }
  void setRepositoryName(const QString &name) { _repositoryName = name; }

  bool isFallback() const override { return false; }
  void setIconUrl(const OmniIconUrl &url) { _url = url; }

  QString repositoryName() const override { return _repositoryName; }
  const QString &repositoryId() { return _repositoryId; }

  virtual std::vector<QString> keywords() const override { return {}; }

  BuiltinCommand() {}
};

class BuiltinCommandRepository : public AbstractCommandRepository {
  std::vector<std::shared_ptr<AbstractCmd>> _commands;

  virtual QString id() const override = 0;
  QString name() const override = 0;
  std::vector<std::shared_ptr<AbstractCmd>> commands() const override final { return _commands; }
  QString author() const override final { return Omnicast::APP_ID; }

protected:
  template <typename T> void registerCommand() {
    auto cmd = std::make_shared<T>();
    cmd->setRepositoryId(id());
    cmd->setRepositoryName(name());
    _commands.emplace_back(cmd);
  }

public:
  BuiltinCommandRepository() {}
};

class CommandDatabase {
  std::vector<std::shared_ptr<AbstractCommandRepository>> _repositories;

public:
  const std::vector<std::shared_ptr<AbstractCommandRepository>> &repositories() const;

  template <typename T> void registerRepository() { _repositories.push_back(std::make_shared<T>()); }

  const AbstractCmd *findCommand(const QString &id);
  const AbstractCommandRepository *findRepository(const QString &name);

  CommandDatabase();
};
