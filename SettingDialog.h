#pragma once

#include <QDialog>
#include "ui_SettingDialog.h"
#include "AppConfig.h"

#include"qmessagebox.h"

class SettingDialog : public QDialog
{
	Q_OBJECT

public:
	SettingDialog(QWidget *parent = Q_NULLPTR);
	~SettingDialog();
	void loadSetting(const AppConfig*);

	Ui::SettingDialog ui;
public slots:
	void backBtnClicked();
	void saveSettingBtnClicked();

private:
};
