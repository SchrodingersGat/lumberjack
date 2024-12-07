#ifndef PLUGINS_DIALOG_HPP
#define PLUGINS_DIALOG_HPP

#include <QDialog>

#include "plugin_registry.hpp"

#include "ui_plugins_dialog.h"

class PluginsDialog : public QDialog
{
    Q_OBJECT

public:
    PluginsDialog(PluginRegistry &r, QWidget *parent = nullptr);

public slots:
    void selectPluginType(int idx);

protected:
    Ui::plugins_dialog ui;

    void initPluginsTable(void);
    void loadPluginsTable(const PluginList &plugins);

    PluginRegistry &registry;

};

#endif // PLUGINS_DIALOG_HPP
