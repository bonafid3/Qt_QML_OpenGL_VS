#include "application.h"

Application* Application::mInstance = nullptr;

Application::Application(QObject* parent) : QObject(parent)
{
	mInstance = this;
}

ComboBoxModel* Application::comboModel()
{
	return &mComboModel;
}
