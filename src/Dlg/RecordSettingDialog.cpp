#include "stdafx.h"
#include "RecordSettingDialog.h"
#include "ConfigManager.h"
#include "ConfigDef.h"

using namespace SoLive::Config;

namespace SoLive::Dlg
{
    RecordSettingDialog::RecordSettingDialog(QWidget* parent)
        : QDialog(parent), _parent(parent)
    {
        setupUi();
        setupConnection();
    }

    void RecordSettingDialog::setupUi()
    {
        // 设置对话框的窗口标志
        setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        setObjectName("recordSettingDialog");

        QVBoxLayout* layout = new QVBoxLayout(this);
        QHBoxLayout* hLayout = new QHBoxLayout(this);
        _selectPathBtnPtr.reset(new QPushButton("修改保存路径", this));
        _selectPathBtnPtr->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        auto& configManager = ConfigManager::instance();
        auto recordSavePath = std::any_cast<std::string>(configManager.getValue(SoLive::Config::CONFIG_RECORD_SAVE_PATH));
        _pathLabelPtr.reset(new QLabel(QString::fromUtf8(recordSavePath.c_str()),this));
        _pathLabelPtr->setStyleSheet("QLabel { color : gray; }");
        _pathLabelPtr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        QSpacerItem* hSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem* vSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        hLayout->addWidget(_pathLabelPtr.get());
        hLayout->addItem(hSpacer);
        hLayout->addWidget(_selectPathBtnPtr.get());
        hLayout->setContentsMargins(20, 0, 0, 0);
        layout->setSpacing(0);
        layout->addLayout(hLayout);
        layout->addItem(vSpacer);
        // 创建 QDialogButtonBox
        QPushButton* cancelButton = new QPushButton(this);
        cancelButton->setText(tr("取消"));
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
        QHBoxLayout* buttonLayout = new QHBoxLayout;
        buttonLayout->addStretch();
        buttonLayout->addWidget(cancelButton);
        buttonLayout->addStretch();
        layout->addLayout(buttonLayout);

    }
    void RecordSettingDialog::setupConnection()
    {
        connect(_selectPathBtnPtr.get(), &QPushButton::clicked, this, &RecordSettingDialog::selectPath);
    }

    void RecordSettingDialog::selectPath()
    {
        auto activeWindow = _parent;
        QFileDialog dialog(activeWindow, tr("打开目录"));
        dialog.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        dialog.setFileMode(QFileDialog::DirectoryOnly);
        dialog.setOption(QFileDialog::ShowDirsOnly);
        dialog.setOption(QFileDialog::DontResolveSymlinks);
        dialog.setOption(QFileDialog::DontUseNativeDialog); // 需要开启，否则不可居中
        dialog.setModal(true);

        QSize parentSize = activeWindow->size();
        int dialogWidth = std::min(parentSize.width() * 0.6, QApplication::desktop()->screenGeometry(activeWindow).width() * 0.8);
        int dialogHeight = std::min(parentSize.height() * 0.6, QApplication::desktop()->screenGeometry(activeWindow).height() * 0.8);
        dialog.show();
        dialog.resize(dialogWidth, dialogHeight);
        QRect screenRect = QApplication::desktop()->screenGeometry(activeWindow);
        QRect dialogRect = dialog.geometry();
        QPoint center = screenRect.center();
        QPoint dialogPos(center.x() - dialogRect.width() / 2, center.y() - dialogRect.height() / 2);
        dialog.move(dialogPos);

        if (dialog.exec() == QFileDialog::Accepted)
        {
            QString dir = dialog.selectedFiles().first();
            if (!dir.isEmpty())
            {
                _pathLabelPtr->setText(dir);
                
                auto& configManager = ConfigManager::instance();
                auto recordSavePath = std::any_cast<std::string>(configManager.getValue(SoLive::Config::CONFIG_RECORD_SAVE_PATH));
                std::string strPath=dir.toUtf8().constData();
                configManager.setValue(SoLive::Config::CONFIG_RECORD_SAVE_PATH, std::any(strPath));
            }
        }
    }
}
