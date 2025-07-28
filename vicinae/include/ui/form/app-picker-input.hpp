#include "app/app-database.hpp"
#include "ui/form/selector-input.hpp"

class AppPickerInput : public SelectorInput {
  const AbstractAppDatabase *m_appDb;

public:
  AppPickerInput(const AbstractAppDatabase *appDb);
};
