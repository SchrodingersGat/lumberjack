#ifndef PLUGINS_DIALOG_HPP
#define PLUGINS_DIALOG_HPP

#include <QDialog>

#include "plugin_base.hpp"
#include "ui_plugins_dialog.h"

class PluginsDialog : public QDialog
{
    Q_OBJECT

public:
    PluginsDialog(QWidget *parent = nullptr);

public slots:
    void selectPluginType(int idx);

protected:
    Ui::plugins_dialog ui;

    void initPluginsTable(void);
    void loadPluginsTable(const PluginList &plugins);

};

#endif // PLUGINS_DIALOG_HPP
