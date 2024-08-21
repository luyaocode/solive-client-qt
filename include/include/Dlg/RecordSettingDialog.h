#ifndef RECORD_SETTING_DIALOG_H
#define RECORD_SETTING_DIALOG_H

#include <QWidget>
#include <QLabel>
namespace SoLive::Dlg
{
    class RecordSettingDialog : public QDialog
    {
        Q_OBJECT

    public:
        explicit RecordSettingDialog(QWidget* parent = nullptr);

    private Q_SLOTS:
        void selectPath();
    private:
        void setupUi();
        void setupConnection();
    private:
        QScopedPointer<QLabel> _pathLabelPtr;
        QScopedPointer<QPushButton> _selectPathBtnPtr;
        QWidget* _parent;
    };
}

#endif // RECORD_SETTING_DIALOG_H
