#ifndef MESHLABWIDGETS_H
#define MESHLABWIDGETS_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QSlider>

#include "common/interfaces.h"
#include "common/filter_parameter/rich_parameter_list.h"

class MeshLabWidget : public QWidget
{
	Q_OBJECT
public:
	MeshLabWidget(QWidget* p, const RichParameter& rpar, const RichParameter& defaultValue);

	// this one is called by resetValue to reset the values inside the widgets.
	virtual void resetWidgetValue() = 0;
	// bring the values from the Qt widgets to the parameter (e.g. from the checkBox to the parameter).
	virtual void collectWidgetValue() = 0;
	virtual void setWidgetValue(const Value& nv) = 0;
	virtual ~MeshLabWidget();

	virtual void addWidgetToGridLayout(QGridLayout* lay,const int r) = 0;
	// called when the user press the 'default' button to reset the parameter values to its default.
	// It just set the parameter value and then it calls the specialized resetWidgetValue() to update also the widget.
	void resetValue();
	// update the parameter with the current widget values and return it.
	const Value& widgetValue();

	QString parameterName() const;

	QLabel* helpLab;

signals:
	void parameterChanged();
protected:
	RichParameter* rp;
	RichParameter* defp;
};

class BoolWidget : public MeshLabWidget
{
public:
	BoolWidget(QWidget* p, const RichBool& rb, const RichBool&rdef);
	~BoolWidget();

	void addWidgetToGridLayout(QGridLayout* lay,const int r);
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);

	QCheckBox* cb;
};

class LineEditWidget : public MeshLabWidget
{
  Q_OBJECT
protected:
	QLabel* lab;
	QLineEdit* lned;
	QString lastVal;

	private slots:
		void changeChecker();
	signals:
		void lineEditChanged();
public:
	LineEditWidget(QWidget* p, const RichParameter& rpar, const RichParameter&rdef);
	~LineEditWidget();
	void addWidgetToGridLayout(QGridLayout* lay,const int r);
	virtual void collectWidgetValue() = 0;
	virtual void resetWidgetValue() = 0;
	virtual void setWidgetValue(const Value& nv) = 0;
};

class IntWidget : public LineEditWidget
{
public:
	IntWidget(QWidget* p, const RichInt& rpar, const RichInt&rdef);
	~IntWidget(){}
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);
};

class FloatWidget : public  LineEditWidget
{
public:
	FloatWidget(QWidget* p, const RichFloat& rpar, const RichFloat& rdef);
	~FloatWidget(){}
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);
};

class StringWidget  : public  LineEditWidget
{
public:
	StringWidget(QWidget* p, const RichString& rpar, const RichString& rdef);
	~StringWidget(){}
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);
};

class ColorWidget : public MeshLabWidget
{
	Q_OBJECT
public:
	ColorWidget(QWidget *p, const RichColor& newColor, const RichColor& rdef);
	~ColorWidget();

	void addWidgetToGridLayout(QGridLayout* lay,const int r);
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);
	void initWidgetValue();

private:
	void  updateColorInfo(const ColorValue& newColor);
private slots:
	void pickColor();
signals:
	void dialogParamChanged();

protected:
	QHBoxLayout* vlay;

private:
	QPushButton* colorButton;
	QLabel* colorLabel;
	QLabel* descLabel;
	QColor pickcol;
};

class AbsPercWidget : public MeshLabWidget
{
	Q_OBJECT
public:
	AbsPercWidget(QWidget *p, const RichAbsPerc& rabs, const RichAbsPerc& rdef);
	~AbsPercWidget();

	void addWidgetToGridLayout(QGridLayout* lay, const int r);
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);

private:
	void  setValue(float val, float minV, float maxV);

public slots:

	void on_absSB_valueChanged(double newv);
	void on_percSB_valueChanged(double newv);
signals:
	void dialogParamChanged();

protected:
	QDoubleSpinBox *absSB;
	QDoubleSpinBox *percSB;
	QLabel* fieldDesc;
	float m_min;
	float m_max;
	QGridLayout* vlay;
};

class Point3fWidget : public MeshLabWidget
{
	Q_OBJECT
public:
	Point3fWidget(QWidget *p, const RichPoint3f& rpf, const RichPoint3f& rdef, QWidget *gla);
	~Point3fWidget();
	QString paramName;
	vcg::Point3f getValue();

	void addWidgetToGridLayout(QGridLayout* lay,const int r);
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);

	public slots:
	void  getPoint();
	void  setValue(QString name, Point3m val);
	void  setShotValue(QString name, Shotm val);
	signals:
	void askViewDir(QString);
	void askViewPos(QString);
	void askSurfacePos(QString);
	void askCameraPos(QString);
	void askTrackballPos(QString);

protected:
	QLineEdit * coordSB[3];
	QComboBox *getPoint3Combo;
	QPushButton *getPoint3Button;
	QLabel* descLab;
	QHBoxLayout* vlay;
};

class Matrix44fWidget : public MeshLabWidget
{
	Q_OBJECT

public:
	Matrix44fWidget(QWidget *p, const RichMatrix44f& rpf, const RichMatrix44f& rdef, QWidget *gla_curr);
	~Matrix44fWidget();
	QString paramName;
	vcg::Matrix44f getValue();

	void addWidgetToGridLayout(QGridLayout* lay,const int r);
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);

public slots:
	void setValue(QString name, Matrix44m val);
	void getMatrix();
	void pasteMatrix();
	void invalidateMatrix(const QString& s);
signals:
	void askMeshMatrix(QString);

protected:
	QLineEdit * coordSB[16];
	QPushButton *getPoint3Button;
	QLabel* descLab;
	QGridLayout* lay44;
	QVBoxLayout* vlay;
	vcg::Matrix44f m;
	bool valid;
};

class ShotfWidget : public MeshLabWidget
{
	Q_OBJECT

public:
	ShotfWidget(QWidget *p, const RichShotf& rpf, const RichShotf& rdef,  QWidget *gla);
	~ShotfWidget();
	QString paramName;
	vcg::Shotf getValue();

	void addWidgetToGridLayout(QGridLayout* lay,const int r);
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);

public slots:
	void  getShot();
	void  setShotValue(QString name, Shotm val);
signals:
	void askRasterShot(QString);
	void askMeshShot(QString);
	void askViewerShot(QString);

protected:
	vcg::Shotf curShot;
	QLineEdit * shotLE;
	QPushButton *getShotButton;
	QComboBox *getShotCombo;
	QLabel* descLab;
	QHBoxLayout* hlay;
};

class DynamicFloatWidget : public MeshLabWidget
{
	Q_OBJECT

public:
	DynamicFloatWidget(QWidget *p, const RichDynamicFloat& rdf, const RichDynamicFloat& rdef);
	~DynamicFloatWidget();

	float getValue();
	void  setValue(float val, float minV, float maxV);

	void addWidgetToGridLayout(QGridLayout* lay,const int r);
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);

public slots:
	void setValue(int newv);
	void setValue();
	void setValue(float newValue);

signals:
	//void valueChanged(int mask);
	void dialogParamChanged();

protected:
	QLineEdit *valueLE;
	QSlider   *valueSlider;
	QLabel* fieldDesc;
	float minVal;
	float maxVal;
	QHBoxLayout* hlay;
private :
	float intToFloat(int val);
	int floatToInt(float val);
};

class ComboWidget : public MeshLabWidget
{
	Q_OBJECT
protected:
	QComboBox *enumCombo;
	QLabel *enumLabel;
public:
	ComboWidget(QWidget *p, const RichParameter& rpar, const RichParameter& rdef);
	~ComboWidget();
	void Init(QWidget *p,int newEnum, QStringList values);
	void addWidgetToGridLayout(QGridLayout* lay,const int r);
	virtual void collectWidgetValue() = 0;
	virtual void resetWidgetValue() = 0;
	virtual void setWidgetValue(const Value& nv) = 0;

	int getIndex();
	void  setIndex(int newEnum);

signals:
	void dialogParamChanged();
};

class EnumWidget : public ComboWidget
{
	Q_OBJECT

public:
	EnumWidget(QWidget *p, const RichEnum& rpar, const RichEnum& rdef);
	~EnumWidget(){};

	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);

	//returns the number of items in the list
	int getSize();
};

class MeshWidget : public ComboWidget
{
private:
	MeshDocument *md;
public:
	MeshWidget(QWidget *p, const RichMesh& defaultMesh, const RichMesh& rdef);
	~MeshWidget(){};

	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);

	MeshModel * getMesh();
	void setMesh(MeshModel * newMesh);
};

class IOFileWidget : public MeshLabWidget
{
	Q_OBJECT

protected:
	IOFileWidget(QWidget* p, const RichParameter& rpar, const RichParameter& rdef);
	~IOFileWidget();

	void  updateFileName(const FileValue& file);

public:
	void addWidgetToGridLayout(QGridLayout* lay,const int r);
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);

protected slots:
	virtual void selectFile() = 0;

signals:
	void dialogParamChanged();


protected:
	QLineEdit* filename;
	QPushButton* browse;
	QString fl;
	QLabel* descLab;
	QHBoxLayout* hlay;
};

class SaveFileWidget : public IOFileWidget
{
	Q_OBJECT
public:
	SaveFileWidget(QWidget* p, const RichSaveFile& rpar, const RichSaveFile& rdef);
	~SaveFileWidget();

protected slots:
	void selectFile();

};

class OpenFileWidget : public IOFileWidget
{
	Q_OBJECT
public:
	OpenFileWidget(QWidget *p, const RichOpenFile& rdf, const RichOpenFile& rdef);
	~OpenFileWidget();

	/*void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);*/

protected slots:
	void selectFile();
};

#endif // MESHLABWIDGETS_H
