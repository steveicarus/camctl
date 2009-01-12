
# include  <qapplication.h>
# include  "CamtoolMain.h"

int main(int argc, char*argv[])
{
      QApplication app(argc, argv);

      CamtoolMain*widget = new CamtoolMain;
      widget->show();

      return app.exec();
}
