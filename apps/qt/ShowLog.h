#ifndef SHOWLOG_H
#define SHOWLOG_H

#include <QDialog>

/// print log data with dialog
class ShowLog : public QDialog {
    Q_OBJECT

public:
    explicit ShowLog(const QList<QString>& log, QWidget *parent = nullptr);
    virtual ~ShowLog() override;
};

#endif
