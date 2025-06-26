#pragma once
#include "common.hpp"
#include "settings/command-metadata-settings-detail.hpp"
#include <qwidget.h>

class ExtensionSettingsDetail : public QWidget {
  QString m_rootItemId;
  std::shared_ptr<AbstractCommandRepository> m_command;
  QJsonObject m_preferenceValues;
  std::map<QString, AbstractPreferenceFormItem *> m_preferenceFields;
  QVBoxLayout *m_layout = new QVBoxLayout;

  void setupUI();
  void savePendingPreferences();

public:
  ExtensionSettingsDetail(const QString &rootItemId,
                          const std::shared_ptr<AbstractCommandRepository> &repository);
  ~ExtensionSettingsDetail();
};
