package fr.soleil.comete.bptApp.cometeWrapper;

import fr.soleil.comete.box.matrixbox.NumberMatrixBox;
import fr.soleil.comete.box.scalarbox.BooleanScalarBox;
import fr.soleil.comete.box.scalarbox.NumberScalarBox;
import fr.soleil.comete.box.scalarbox.ScalarCometeBox;
import fr.soleil.comete.box.scalarbox.StringScalarBox;
import fr.soleil.comete.swing.Button;
import fr.soleil.comete.swing.IconButton;
import fr.soleil.comete.swing.ImageViewer;
import fr.soleil.comete.swing.Label;
import fr.soleil.comete.swing.StringButton;
import fr.soleil.comete.swing.TextArea;
import fr.soleil.comete.swing.TextField;
import fr.soleil.comete.swing.WheelSwitch;
import fr.soleil.comete.tango.data.service.TangoKey;
import fr.soleil.comete.tango.data.service.TangoKeyTool;


/****************************************************************
 *  TangoConnection 
 *  
 *  To mask every COMETE connections
 * **************************************************************/
public class TangoConnection {
	
	public final String stringType = "string";
	public final String booleanType = "boolean";
	public final String numberType = "number";
	public final String numberMatrixType = "numberMatrix";
	
	/****************************************************************
	 *  connectAttribute 
	 *  
	 *  For a Label component
	 * **************************************************************/
	public void connectAttribute(String tangoDataType, String deviceAdress, String attributeName, Label component, boolean metaData){
		ScalarCometeBox box = getBoxWithType(tangoDataType);
		if (box == null)
			return;
						
		TangoKey tangoKey = new TangoKey();
		if(attributeName.equals("")) //state specific case
			TangoKeyTool.registerAttribute(tangoKey, deviceAdress);
		else
			TangoKeyTool.registerAttribute(tangoKey, deviceAdress, attributeName);

//		if (metaData)
//			box.connectWidget(component, tangoKey);
//		else 
//			box.connectWidgetNoMetaData(component, tangoKey);
		if(!metaData) {
			box.setColorEnabled(component, false);
		}
		box.connectWidget(component, tangoKey);
	}
	/****************************************************************
	 *  connectAttribute 
	 *  
	 *  For a WheelSwitch component
	 * **************************************************************/
	public void connectAttribute(String tangoDataType, String deviceAdress, String attributeName, WheelSwitch component, boolean metaData){
		ScalarCometeBox box = getBoxWithType(tangoDataType);
		if (box == null)
			return;
		TangoKey tangoKey = new TangoKey();
		TangoKeyTool.registerAttribute(tangoKey, deviceAdress, attributeName);
		if (metaData)
			box.connectWidget(component, tangoKey);
		else 
			box.connectWidgetNoMetaData(component, tangoKey);
	}
	/****************************************************************
	 *  connectAttribute
	 *  
	 *  For a IconButton component
	 * **************************************************************/
	public void connectAttribute(String tangoDataType, String deviceAdress, String attributeName, Button component, boolean metaData){
		ScalarCometeBox box = getBoxWithType(tangoDataType);
		if (box == null)
			return;
		TangoKey tangoKey = new TangoKey();
		TangoKeyTool.registerAttribute(tangoKey, deviceAdress, attributeName);
		if (metaData)
			box.connectWidget(component, tangoKey);
		else 
			box.connectWidgetNoMetaData(component, tangoKey);
	}
	/****************************************************************
	 *  connectAttribute
	 *  
	 *  For a IconButton component
	 * **************************************************************/
	public void connectAttribute(String tangoDataType, String deviceAdress, String attributeName, IconButton component, boolean metaData){
		ScalarCometeBox box = getBoxWithType(tangoDataType);
		if (box == null)
			return;
		TangoKey tangoKey = new TangoKey();
		TangoKeyTool.registerAttribute(tangoKey, deviceAdress, attributeName);
		if (metaData)
			box.connectWidget(component, tangoKey);
		else 
			box.connectWidgetNoMetaData(component, tangoKey);
	}
	/****************************************************************
	 *  connectAttribute 
	 *  
	 *  For a TextField component
	 * **************************************************************/
	public void connectAttribute(String tangoDataType, String deviceAdress, String attributeName, TextField component, boolean metaData){
		ScalarCometeBox box = getBoxWithType(tangoDataType);
		if (box == null)
			return;
		TangoKey tangoKey = new TangoKey();
		if(attributeName.equals("")) //state specific case
			TangoKeyTool.registerAttribute(tangoKey, deviceAdress);
		else
			TangoKeyTool.registerAttribute(tangoKey, deviceAdress, attributeName);
		
		if (metaData)
			box.connectWidget(component, tangoKey);
		else 
			box.connectWidgetNoMetaData(component, tangoKey);
	}
	/****************************************************************
	 *  connectAttribute 
	 *  
	 *  For a TextArea component
	 * **************************************************************/
	public void connectAttribute(String tangoDataType, String deviceAdress, String attributeName, TextArea component, boolean metaData){
		ScalarCometeBox box = getBoxWithType(tangoDataType);
		if (box == null)
			return;
		TangoKey tangoKey = new TangoKey();
		if(attributeName.equals("")) //state specific case
			TangoKeyTool.registerAttribute(tangoKey, deviceAdress);
		else
			TangoKeyTool.registerAttribute(tangoKey, deviceAdress, attributeName);
		if (metaData)
			box.connectWidget(component, tangoKey);
		else 
			box.connectWidgetNoMetaData(component, tangoKey);
	}
	/****************************************************************
	 *  connectAttribute 
	 *  
	 *  For a ImageViewer component
	 * **************************************************************/
	public void connectAttribute(String deviceAdress, String attributeName, ImageViewer component){
		NumberMatrixBox boxMat = new NumberMatrixBox();
			 
        TangoKey bptStatekey = new TangoKey();
        TangoKeyTool.registerAttribute(bptStatekey, deviceAdress, attributeName);
        boxMat.connectWidget(component, bptStatekey);
	}
	/****************************************************************
	 *  connectCommand 
	 *  
	 * 	For a StringButton component 
	 * **************************************************************/
	public void connectCommand(String tangoDataType, String deviceAdress, String attributeName, StringButton component, boolean metaData){
		ScalarCometeBox box = getBoxWithType(tangoDataType);
		if (box == null)
			return;
		TangoKey tangoKey = new TangoKey();
		TangoKeyTool.registerCommand(tangoKey, deviceAdress, attributeName);
		if (metaData)
			box.connectWidget(component, tangoKey);
		else 
			box.connectWidgetNoMetaData(component, tangoKey);
	}
	
	
	/****************************************************************
	 *  connectProperty 
	 *  
	 *  For a Label component
	 * **************************************************************/
	public void connectProperty(String tangoDataType, String deviceAdress, String propertyName, Label component){
		ScalarCometeBox box = getBoxWithType(tangoDataType);
		if (box == null)
			return;		 
        TangoKey bptStatekey = new TangoKey();
        TangoKeyTool.registerDeviceProperty(bptStatekey, deviceAdress, propertyName);
        box.connectWidget(component, bptStatekey);
	}
	/****************************************************************
	 *  connectProperty 
	 *  
	 *  For a WheelSwitch component
	 * **************************************************************/
	public void connectProperty(String tangoDataType, String deviceAdress, String propertyName, WheelSwitch component){
		ScalarCometeBox box = getBoxWithType(tangoDataType);
		if (box == null)
			return;		 
        TangoKey bptStatekey = new TangoKey();
        TangoKeyTool.registerDeviceProperty(bptStatekey, deviceAdress, propertyName);
        box.connectWidget(component, bptStatekey);
	}
	/****************************************************************
	 *  connectDevice
	 *  
	 * **************************************************************/
	public boolean connectLabelDeviceClass(String deviceAdress, Label component){
		StringScalarBox boxStr = new StringScalarBox();  
        TangoKey bptStatekey = new TangoKey();
        try{
        	TangoKeyTool.registerDeviceClass(bptStatekey, deviceAdress);
        	boxStr.connectWidget(component, bptStatekey);
        	 return true;
        }catch (Exception E){
        	return false;
        }
	}
	/****************************************************************
	 *  getBoxType 
	 *  
	 * **************************************************************/
	private ScalarCometeBox getBoxWithType(String tangoDataType){
		ScalarCometeBox box;

		if (tangoDataType.equals(stringType))
			box = new StringScalarBox(); 
		else if (tangoDataType.equals(booleanType))
			box = new BooleanScalarBox();
		else if (tangoDataType.equals(numberType))
			box = new NumberScalarBox();
		else 
			return null;
		return box;
	}
	
}
