#include "PropertiesDock.h"
#include <qwidget.h>
#include <qboxlayout.h>
#include <QDoubleSpinBox>
#include <QLabel>
#include <qscrollarea.h>
#include <QCheckBox>
#include <QLineEdit>
#include "Actors/Actor.h"
#include "Components/Component.h"
#include "Render/PipelineObjects.h"
#include "PropertyWidgets/BoolWidget.h" 
#include "PropertyWidgets/Float2Widget.h"
#include "PropertyWidgets/Float3Widget.h"
#include "PropertyWidgets/Float4Widget.h"
#include "PropertyWidgets/FloatWidget.h"
#include "PropertyWidgets/IntWidget.h"
#include "PropertyWidgets/UintWidget.h"
#include "PropertyWidgets/VectorWidget.h"
#include "PropertyWidgets/StringWidget.h"
#include "PropertyWidgets/WStringWidget.h"
#include "PropertyWidgets/TextureDataWidget.h"
#include "PropertyWidgets/ShaderDataWidget.h"
#include "PropertyWidgets/MeshComponentDataWidget.h"
#include "PropertyWidgets/TransformWidget.h"
#include "PropertyWidgets/VEnumWidget.h"
#include "VEnum.h"
#include "Render/Material.h"
#include "Render/RenderTypes.h"

PropertiesDock::PropertiesDock() : QDockWidget("Properties")
{
    setFixedWidth(400);

    actorPropsScrollArea = new QScrollArea();
    actorPropsGridLayout = new QGridLayout();
    actorPropsWidget = new QWidget();
    actorPropsWidget->setFixedWidth(400);

    actorPropsWidget->setLayout(actorPropsGridLayout);
    actorPropsScrollArea->setWidget(actorPropsWidget);
    setWidget(actorPropsScrollArea);
}

void PropertiesDock::DisplayActorProperties(Actor* actor)
{
    //Set the inner values in the Qt widgets instead of remaking all the widgets below
    if (previousActor == actor)
    {
        ResetPropertyWidgetValues();
        return;
    }

    if (actorPropsWidget && actorPropsGridLayout)
    {
        propertyWidgetsToUpdate.clear();
        delete actorPropsGridLayout;
        delete actorPropsWidget;
    }

    actorPropsWidget = new QWidget();
    actorPropsGridLayout = new QGridLayout();
    actorPropsGridLayout->setAlignment(Qt::AlignTop);

    //Go over actor properties
    int gridRow = 0;

    for (auto& props : actor->GetAllProps())
    {
        IterateOverProperties(props, gridRow);
    }

    actorPropsWidget->setLayout(actorPropsGridLayout);
    actorPropsWidget->setFixedWidth(400);

    actorPropsScrollArea->setWidget(actorPropsWidget);

    setWidget(actorPropsScrollArea);

    previousActor = actor;
}

void PropertiesDock::IterateOverProperties(Properties& props, int& currentGridRow)
{
    const int propertyNameColumn = 0;
    const int propertyDataColumn = 1;

    //Set title to seperate components/actor properties
    auto titleLabel = new QLabel(props.title.c_str(), this);
    titleLabel->setStyleSheet("font-weight: bold");
    actorPropsGridLayout->addWidget(titleLabel, currentGridRow, 0, 1, 2, Qt::AlignCenter);
    currentGridRow++;

    //Properties
    for (auto& prop : props.propMap)
    {
        if (prop.second.hide)
        {
            continue;
        }

        const std::string& name = prop.first;

        //Set property name onto label
        actorPropsGridLayout->addWidget(new QLabel(name.c_str()), currentGridRow, propertyNameColumn);

        //Set property data into grid
        if (props.CheckType<bool>(name))
        {
            auto boolWidget = new BoolWidget(props.GetData<bool>(name));
            actorPropsGridLayout->addWidget(boolWidget, currentGridRow, propertyDataColumn);
            propertyWidgetsToUpdate.push_back((IPropertyWidget*)boolWidget);
        }
        else if (props.CheckType<int>(name))
        {
            auto intWidget = new IntWidget(props.GetData<int>(name));
            actorPropsGridLayout->addWidget(intWidget, currentGridRow, propertyDataColumn);
            propertyWidgetsToUpdate.push_back((IPropertyWidget*)intWidget);
        }
        else if (props.CheckType<uint32_t>(name))
        {
            auto intWidget = new UintWidget(props.GetData<uint32_t>(name), props.GetProperty(name)->readOnly);
            actorPropsGridLayout->addWidget(intWidget, currentGridRow, propertyDataColumn);
            propertyWidgetsToUpdate.push_back((IPropertyWidget*)intWidget);
        }
        else if (props.CheckType<float>(name))
        {
            auto floatWidget = new FloatWidget(props.GetData<float>(name));
            actorPropsGridLayout->addWidget(floatWidget, currentGridRow, propertyDataColumn);
            propertyWidgetsToUpdate.push_back((IPropertyWidget*)floatWidget);
        }
        else if (props.CheckType<XMFLOAT3>(name))
        {
            auto float3Widget = new Float3Widget(props.GetData<XMFLOAT3>(name));
            actorPropsGridLayout->addWidget(float3Widget, currentGridRow, propertyDataColumn);
            propertyWidgetsToUpdate.push_back((IPropertyWidget*)float3Widget);
        }      
        else if (props.CheckType<XMFLOAT4>(name))
        {
            auto float4Widget = new Float4Widget(props.GetData<XMFLOAT4>(name));
            actorPropsGridLayout->addWidget(float4Widget, currentGridRow, propertyDataColumn);
            propertyWidgetsToUpdate.push_back((IPropertyWidget*)float4Widget);
        }
        else if (props.CheckType<XMFLOAT2>(name))
        {
            auto float2Widget = new Float2Widget(props.GetData<XMFLOAT2>(name));
            actorPropsGridLayout->addWidget(float2Widget, currentGridRow, propertyDataColumn);
            propertyWidgetsToUpdate.push_back((IPropertyWidget*)float2Widget);
        }
        else if (props.CheckType<XMVECTOR>(name))
        {
            auto vectorWidget = new VectorWidget(props.GetData<XMVECTOR>(name));
            actorPropsGridLayout->addWidget(vectorWidget, currentGridRow, propertyDataColumn);
            propertyWidgetsToUpdate.push_back((IPropertyWidget*)vectorWidget);
        }
        else if (props.CheckType<std::string>(name))
        {
            auto stringWidget = new StringWidget(prop.second);
            actorPropsGridLayout->addWidget(stringWidget, currentGridRow, propertyDataColumn);
            propertyWidgetsToUpdate.push_back((IPropertyWidget*)stringWidget);
        }    
        else if (props.CheckType<std::wstring>(name))
        {
            auto wstringWidget = new WStringWidget(prop.second);
            actorPropsGridLayout->addWidget(wstringWidget, currentGridRow, propertyDataColumn);
            propertyWidgetsToUpdate.push_back((IPropertyWidget*)wstringWidget);
        }       
        else if (props.CheckType<TextureData>(name))
        {
            auto textureDataWidget = new TextureDataWidget(prop.second);
            actorPropsGridLayout->addWidget(textureDataWidget, currentGridRow, propertyDataColumn);
            propertyWidgetsToUpdate.push_back((IPropertyWidget*)textureDataWidget);
        }
        else if (props.CheckType<ShaderData>(name))
        {
            auto shaderDataWidget = new ShaderDataWidget(prop.second);
            actorPropsGridLayout->addWidget(shaderDataWidget, currentGridRow, propertyDataColumn);
            propertyWidgetsToUpdate.push_back((IPropertyWidget*)shaderDataWidget);
        }     
        else if (props.CheckType<MeshComponentData>(name))
        {
            auto meshComponentDataWidget = new MeshComponentDataWidget(prop.second);
            actorPropsGridLayout->addWidget(meshComponentDataWidget, currentGridRow, propertyDataColumn);
            propertyWidgetsToUpdate.push_back((IPropertyWidget*)meshComponentDataWidget);
        }
        else if (props.CheckType<Transform>(name))
        {
            auto transformWidget = new TransformWidget((Transform*)prop.second.data, this);
            actorPropsGridLayout->addWidget(transformWidget, currentGridRow, propertyDataColumn);
            propertyWidgetsToUpdate.push_back((IPropertyWidget*)transformWidget);
        }  
        else if (props.CheckType<VEnum>(name))
        {
            auto vEnumWidget = new VEnumWidget(prop.second);
            actorPropsGridLayout->addWidget(vEnumWidget, currentGridRow, propertyDataColumn);
            propertyWidgetsToUpdate.push_back((IPropertyWidget*)vEnumWidget);
        }

        currentGridRow++;
    }
}

void PropertiesDock::Clear()
{
    previousActor = nullptr;
    propertyWidgetsToUpdate.clear();

    if (actorPropsGridLayout && actorPropsWidget)
    {
        delete actorPropsGridLayout;
        actorPropsGridLayout = nullptr;

        delete actorPropsWidget;
        actorPropsWidget = nullptr;
    }
}

void PropertiesDock::ResetPropertyWidgetValues()
{
    for (IPropertyWidget* propertyWidget : propertyWidgetsToUpdate)
    {
        propertyWidget->ResetValue();
    }
}
