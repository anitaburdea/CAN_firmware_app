#ifndef FILE_LOCATION_H
#define FILE_LOCATION_H
class file_Location : public QObject
{
    Q_OBJECT
public:
    explicit file_Location( QObject *parent = nullptr);

signals:
    void locationReceived(QByteArray data);
private slots:
private:

};

#endif // FILE_LOCATION_H
