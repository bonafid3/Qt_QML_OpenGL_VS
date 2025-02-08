#pragma once
#include <QObject>
#include <QString>

#include "texturecombomodel.h"

class Application : public QObject
{
	Q_OBJECT
	Q_PROPERTY(ComboBoxModel* comboModel READ comboModel CONSTANT)

public:
	
	static Application* instance() { return mInstance; }

	explicit Application(QObject* parent = nullptr);

	[[nodiscard]] ComboBoxModel* comboModel();

private:
	static Application* mInstance;
	ComboBoxModel mComboModel;

};
