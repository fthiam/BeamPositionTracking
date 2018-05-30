package main.java.fr.soleil.comete.bptApp.view;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import javax.swing.BorderFactory;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.WindowConstants;
import javax.swing.border.Border;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import fr.soleil.comete.box.matrixbox.NumberMatrixBox;
import fr.soleil.comete.box.scalarbox.BooleanScalarBox;
import fr.soleil.comete.box.scalarbox.NumberScalarBox;
import fr.soleil.comete.box.scalarbox.StringScalarBox;
import fr.soleil.comete.definition.widget.util.CometeColor;
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
 *  PUBLIC CLASS ControlPanel
 * **************************************************************/
public class ControlPanel implements Runnable  {
	private static final String SENSOR_TYPE_PROP = "SensorPluginType";
	private static final String DEVICE_STATUS = "status";
	private static final String IMAGE_ATTR = "thresholdedImage";
    private static final String START_BEAM_TRACKING_CMD = "StartBeamTracking";
    private static final String STOP_BEAM_TRACKING_CMD = "StopBeamTracking";
    private static final String PERCENTAGE_DETECTION = "percentageDetection";
    private static final String X_POSITION_ATTR = "xPosition";
    private static final String Y_POSITION_ATTR = "yPosition";
    private static final String X_AXIS_CAL_ATTR = "isXCalibrated";
    private static final String Y_AXIS_CAL_ATTR = "isYCalibrated";
    private static final String X_TARGET_ATTR = "xAxisTarget";
    private static final String Y_TARGET_ATTR = "yAxisTarget";
    private static final String X_WARN_ZONE_ATTR = "warningZoneXCenter";
    private static final String Y_WARN_ZONE_ATTR = "warningZoneYCenter";
    private static final String WARN_ZONE_RADIUS_ATTR = "warningZoneRadius";
    private static final String BEAM_X_CENTROID = "xAxisCurrentBeamPosition";
    private static final String BEAM_Y_CENTROID = "yAxisCurrentBeamPosition";
    private static final String LOCK_MODE = "fixedMode";
    
    private static final String DEVICE_LOCK_MODE_STR = "Static positionning mode!";
    private static final String DEVICE_DYN_MODE_STR = "Dynamic positionning mode!";
    
    private static final String IMAGE_SENSOR_TYPE = "Lima"; //TO change : plugin name 
    
    
    String _bptDeviceAdress;
    boolean _specificPropertiesInitialized;
	boolean _bptReadyToOperate;
	boolean _bptOperating;	
	ConfigPanel _configPanel;
	JFrame _mainFrame;
	String _xAlias;
	String _yAlias;
	int _xOldFrameDim;
	int _yOldFrameDim;
	int _xOldtarget;
	int _yOldtarget;
	Label _sensorType;
	//COMPONENTS - LEFT PANEL
	ImageViewer _viewer;
	StringButton _configPanelButton;
	StringButton _startBTButton;
	StringButton _stopBTButton;
	Label _xAxisState;
	TextField _xAxisPosition;
	IconButton _isXAxisCalibrated;
	Label _yAxisState;
	TextField _yAxisPosition;
	IconButton _isYAxisCalibrated;
	
	//COMPONENTS - RIGHT PANEL
	JPanel _detectorPanel = new JPanel(new GridLayout(2,0));
	Label _bptState;
	TextArea _bptStatus;
	Label _asState;
	TextArea _asStatus;
	WheelSwitch _percentageDetection;
	WheelSwitch _xTarget;
	WheelSwitch _yTarget;
	WheelSwitch _xWarnCenter;
	WheelSwitch _yWarnCenter;
	WheelSwitch _warnRadius;
	Label _xCentroid;
	Label _yCentroid;
	IconButton _lockModeStatus;
	
	Label lockStatus;
	boolean _lockMode;
	
	
	/****************************************************************
	 *  ControlPanel (String bptDeviceAdress, String asDeviceAdress, ConfigPanel configPanel)
	 *  
	 *  To launch control panel
	 * **************************************************************/
	public ControlPanel (String bptDeviceAdress, String asDeviceAdress, ConfigPanel configPanel, String xAlias, String yAlias){
		_bptDeviceAdress = bptDeviceAdress;
		_configPanel = configPanel;
		_bptReadyToOperate = false;
		_specificPropertiesInitialized = false;
		_bptOperating = false;
		_lockMode = true;
		_xAlias = xAlias;
		_yAlias = yAlias;
		_xOldFrameDim = 0;
		_yOldFrameDim = 0;
		_configPanel.hideConfigPanel();
	
        initComponents();
        initTangoConnexions(bptDeviceAdress, asDeviceAdress); 
		initGui();
		
	}
	/****************************************************************
	 *  updateDevices (String bptDeviceAdress, String asDeviceAdress)
	 *  
	 *  To update devices in use
	 * **************************************************************/
	public boolean updateDevices(String bptDeviceAdress, String asDeviceAdress){
		try{
			initTangoConnexions(bptDeviceAdress, asDeviceAdress); 
			return true;
		}catch(Exception E){
			System.out.println("FAIL TO UPDATE TANGO CONNEXIONS");
			return false;
		}
	}
	/****************************************************************
	 *  refreshInterface ()
	 *  
	 *  To refresh interface 
	 * **************************************************************/
	public void refreshInterface(){
		refreshImageSize();
		refreshStatesColors();
		refreshBPTState();
		refreshApplicationCommands();
		if(!_specificPropertiesInitialized)
			initSpecificProperties();
		if (!_lockMode)
			getNewSelectedTargetOnImage();
		updateTargetDraw();
	}
	/****************************************************************
	 *  initSpecificProperties ()
	 *  
	 *  To init specific properties
	 *  +> don't know exactly when sensor type property will be connected...
	 * **************************************************************/
	public void initSpecificProperties(){
		//If sensor = image type
		if(_sensorType.getText().equals(IMAGE_SENSOR_TYPE)){
	        NumberScalarBox numberBox = new NumberScalarBox();
			TangoKey percentageDetectionKey = new TangoKey();
	        TangoKeyTool.registerAttribute(percentageDetectionKey, _bptDeviceAdress, PERCENTAGE_DETECTION);
	        numberBox.connectWidgetNoMetaData(_percentageDetection, percentageDetectionKey);
	        Label thresholdLabel = new Label();
	        thresholdLabel.setText("- Threshold (%) :");
	        JPanel thresholdPanel = new JPanel(new GridLayout(2, 0));
	        thresholdPanel.add(thresholdLabel);
	        thresholdPanel.setBorder(BorderFactory.createTitledBorder("IMAGE THRESHOLD" ));
	        thresholdPanel.add(_percentageDetection);
			_detectorPanel.add(thresholdPanel);
			_specificPropertiesInitialized = true;
		}
		
	}
	/****************************************************************
	 *  refreshImageSize ()
	 *  
	 *  Will refresh image size only if mainFrame dimensions has changed
	 * **************************************************************/
	public void refreshImageSize(){
		
		//TESTS...
		_xTarget.setSize(200, 200);
		_xTarget.repaint();
		
		int newXFrameDim = _mainFrame.getSize().width;
		int newYFrameDim = _mainFrame.getSize().height;
		
		if (newXFrameDim!=_xOldFrameDim || newYFrameDim!=_yOldFrameDim){

			_xOldFrameDim = newXFrameDim;
			_yOldFrameDim = newYFrameDim;
			//Update image size
			_viewer.setAutoZoom(true);
			//_viewer.zoomIn();
		}
	}
	/****************************************************************
	 *  getNewSelectedTargetOnImage ()
	 *  
	 *  
	 * **************************************************************/
	public void getNewSelectedTargetOnImage(){

		//Get Selected coordinates 
		int xNewTarget = (int)_viewer.getSelectedX();
		int yNewTarget = (int)_viewer.getSelectedY();
		boolean newTarget = false;
		//Check if there is a new target 
		if((xNewTarget != _xOldtarget) || (yNewTarget != _yOldtarget)){
			_yOldtarget = yNewTarget;
			_xOldtarget = xNewTarget;
			newTarget = true;
		}
		//If a new target has been registered, notify user before to send axes...
		if(newTarget){
		 int confirmNewTarget = JOptionPane.showConfirmDialog (null, 
		    								"A new target has been selected on image : \nX = " + 
											xNewTarget + ", Y = " + 
    										yNewTarget + 
    										"\nSend beam to this new target?","Warning",
    										JOptionPane.YES_NO_OPTION);
		    if (confirmNewTarget == JOptionPane.YES_OPTION) { 
		    	//User confirmation -> sending beam to the new target
				_xTarget.setValue(xNewTarget, true, true);
				_yTarget.setValue(yNewTarget, true, true);
		    }
		}

	}
	/****************************************************************
	 *  updateTargetDraw ()
	 *  
	 *  
	 * **************************************************************/
	public void updateTargetDraw(){
		//draw target on image
		_viewer.setBeamPointEnabled(true);
		double []point = {_xTarget.getValue(), _yTarget.getValue()};
		_viewer.setBeamPoint(point);
		_viewer.getBeamPoint();
		_viewer.setBeamColor(Color.black);
		_viewer.setDrawBeamPosition(true);
	}
	/****************************************************************
	 *  refreshBPTState ()
	 *  
	 *  
	 * **************************************************************/
	public void refreshBPTState(){
		String bptStateStr = _bptState.getText();
		
		if (bptStateStr.equals("ON")){
			_bptReadyToOperate = true;
			_bptOperating = false;
		}
		else if (bptStateStr.equals("RUNNING")){
			_bptReadyToOperate = false;
			_bptOperating = true;
		}
		else {
			_bptReadyToOperate = false;
			_bptOperating = false;
		}
		
		//refresh lock state 
		_lockMode = _lockModeStatus.getLabel().equals("TRUE");
		if (_lockMode){
			lockStatus.setText(DEVICE_LOCK_MODE_STR);
			_xTarget.setEditable(false);
			_yTarget.setEditable(false);
			_warnRadius.setEditable(false);
			_xWarnCenter.setEditable(false);
			_yWarnCenter.setEditable(false);
			_percentageDetection.setEditable(false);
		}
		else{
			lockStatus.setText(DEVICE_DYN_MODE_STR);
			_xTarget.setEditable(true);
			_yTarget.setEditable(true);
			_warnRadius.setEditable(true);
			_xWarnCenter.setEditable(true);
			_yWarnCenter.setEditable(true);
			_percentageDetection.setEditable(true);
		}
	}
	/****************************************************************
	 *  refreshApplicationCommands ()
	 *  
	 *  
	 * **************************************************************/
	public void refreshApplicationCommands(){
		if (_bptReadyToOperate){
			_startBTButton.setVisible(true);
			_stopBTButton.setVisible(false);
		}else if (_bptOperating){
			_startBTButton.setVisible(false);
			_stopBTButton.setVisible(true);
		}else{
			_startBTButton.setVisible(false);
			_stopBTButton.setVisible(false);
		}
	}
	/****************************************************************
	 *  refreshStatesColors ()
	 *  
	 *  
	 * **************************************************************/
	public void refreshStatesColors(){
		String xState = _xAxisState.getText();
		String yState = _yAxisState.getText();
		
		if (xState.equals("ON"))
			_xAxisState.setCometeBackground(CometeColor.GREEN);
		else if (xState.equals("MOVING"))
			_xAxisState.setCometeBackground(CometeColor.CYAN);
		else 
			_xAxisState.setCometeBackground(CometeColor.ORANGE);
		
		if (yState.equals("ON"))
			_yAxisState.setCometeBackground(CometeColor.GREEN);
		else if (yState.equals("MOVING"))
			_yAxisState.setCometeBackground(CometeColor.CYAN);
		else 
			_yAxisState.setCometeBackground(CometeColor.ORANGE);
	}
	/****************************************************************
	 *  initComponents() 
	 *  
	 *  To init components
	 * **************************************************************/
	private void initComponents(){
		//Image component - LEFT PANEL
		_viewer = new ImageViewer(){
            private static final long serialVersionUID = 8555814457975234626L;

            @Override
            protected void initSelectionMenu() {
                super.initSelectionMenu();
                selectionMenu.add(containerFactory.createMenuItem(getAction(MODE_ANGLE_ACTION)));
            }
        };
        //Control access
        _configPanelButton = new StringButton();
        _configPanelButton.setText("BPT Configuration");
        _configPanelButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				_configPanel.showConfigPanel();
				hideControlPanel();
			}
        });
        
        _sensorType = new Label();
        
        //BPT State/Status
        _bptState = new Label();
        _bptStatus = new TextArea();
        //AS State/Status
    	_asState = new Label();
    	_asStatus = new TextArea();
        //start beam tracking button 
		_startBTButton = new StringButton();
        _startBTButton.setText(START_BEAM_TRACKING_CMD);
        //stop beam tracking button
        _stopBTButton = new StringButton();
        _stopBTButton.setText(STOP_BEAM_TRACKING_CMD);
        //States
        _xAxisState = new Label();
		_yAxisState = new Label();
		//Axes positions 
		_xAxisPosition = new TextField();
		_yAxisPosition = new TextField();
		//Axes calibration state
		_isXAxisCalibrated = new IconButton();
		_isYAxisCalibrated = new IconButton();
		//Image component - RIGHT PANEL
		//Percentage detection
		_percentageDetection = new WheelSwitch();
		//Beam targets
		_xTarget = new WheelSwitch();
		_yTarget = new WheelSwitch();
		//Warning zone
		_xWarnCenter = new WheelSwitch();
		_yWarnCenter = new WheelSwitch();
		_warnRadius = new WheelSwitch();
		//Beam Centroid
		_xCentroid = new Label();
		_yCentroid = new Label();
		//lock mode
		_lockModeStatus = new IconButton();
		lockStatus = new Label();
	}
	
	/****************************************************************
	 *  initTangoConnexions() 
	 *  
	 *  To init tango connexions
	 * **************************************************************/
	private void initTangoConnexions(String bptDeviceAdress, String asDeviceAdress){
		  			
		//Connect image attribute
		TangoKey imageViewkey = new TangoKey();
        TangoKeyTool.registerAttribute(imageViewkey, bptDeviceAdress, IMAGE_ATTR);
        NumberMatrixBox boxMat = new NumberMatrixBox();
        boxMat.connectWidget(_viewer, imageViewkey);
        StringScalarBox boxStr = new StringScalarBox();  
        BooleanScalarBox booleanBox = new BooleanScalarBox();
        NumberScalarBox numberBox = new NumberScalarBox();
       
        //Sensor type => used to initialized specific stuff in UI
  		TangoKey sensorTypeKey = new TangoKey();
  		TangoKeyTool.registerDeviceProperty(sensorTypeKey, _bptDeviceAdress, SENSOR_TYPE_PROP);
  		boxStr.connectWidgetNoMetaData(_sensorType, sensorTypeKey);  
        //Connect BPT state
        TangoKey bptStatekey = new TangoKey();
        TangoKeyTool.registerAttribute(bptStatekey, bptDeviceAdress+ "/state");
        boxStr.connectWidget(_bptState, bptStatekey);
        //Connect BPT status
        TangoKey bptStatuskey = new TangoKey();
        TangoKeyTool.registerAttribute(bptStatuskey, bptDeviceAdress, DEVICE_STATUS);
        boxStr.connectWidgetNoMetaData(_bptStatus, bptStatuskey);
        //Connect AS state
        TangoKey asStateKey = new TangoKey();
        TangoKeyTool.registerAttribute(asStateKey, asDeviceAdress+ "/state");
        boxStr.connectWidget(_asState, asStateKey);
        //Connect AS status
        TangoKey asStatusKey = new TangoKey();
        TangoKeyTool.registerAttribute(asStatusKey, asDeviceAdress, DEVICE_STATUS);
        boxStr.connectWidgetNoMetaData(_asStatus, asStatusKey);    	
    	
        //Connect startBeamTracking command
		TangoKey startBTCMDkey = new TangoKey();
        TangoKeyTool.registerCommand(startBTCMDkey, bptDeviceAdress, START_BEAM_TRACKING_CMD);
        boxStr.connectWidgetNoMetaData(_startBTButton, startBTCMDkey);
        //Connect startBeamTracking command
      	TangoKey stopBTCMDkey = new TangoKey();
        TangoKeyTool.registerCommand(stopBTCMDkey, bptDeviceAdress, STOP_BEAM_TRACKING_CMD);
        boxStr.connectWidgetNoMetaData(_stopBTButton, stopBTCMDkey);
        //Connect axes states Labels
        TangoKey xAxisStatekey = new TangoKey();
        TangoKeyTool.registerAttribute(xAxisStatekey, asDeviceAdress, "xState");
        boxStr.connectWidgetNoMetaData(_xAxisState, xAxisStatekey);
        TangoKey yAxisStatekey = new TangoKey();
        TangoKeyTool.registerAttribute(yAxisStatekey, asDeviceAdress+"/yState");
        boxStr.connectWidgetNoMetaData(_yAxisState, yAxisStatekey);
        //Connect axes positions Labels
        TangoKey xAxisPoskey = new TangoKey();
        TangoKeyTool.registerAttribute(xAxisPoskey, asDeviceAdress, X_POSITION_ATTR);
        boxStr.connectWidgetNoMetaData(_xAxisPosition, xAxisPoskey);
        TangoKey yAxisPoskey = new TangoKey();
        TangoKeyTool.registerAttribute(yAxisPoskey, asDeviceAdress, Y_POSITION_ATTR);
        boxStr.connectWidgetNoMetaData(_yAxisPosition, yAxisPoskey);
        //Connect axes Calibration states
		TangoKey xAxisCalkey = new TangoKey();
        TangoKeyTool.registerAttribute(xAxisCalkey, asDeviceAdress, X_AXIS_CAL_ATTR);
        booleanBox.connectWidgetNoMetaData(_isXAxisCalibrated, xAxisCalkey);
        TangoKey yAxisCalkey = new TangoKey();
        TangoKeyTool.registerAttribute(yAxisCalkey, asDeviceAdress, Y_AXIS_CAL_ATTR);
        booleanBox.connectWidgetNoMetaData(_isYAxisCalibrated, yAxisCalkey);
        //Connect xTarget 
        TangoKey xTargetKey = new TangoKey();
        TangoKeyTool.registerAttribute(xTargetKey, bptDeviceAdress, X_TARGET_ATTR);
        numberBox.connectWidgetNoMetaData(_xTarget, xTargetKey);
        //Connect yTarget 
        TangoKey yTargetKey = new TangoKey();
        TangoKeyTool.registerAttribute(yTargetKey, bptDeviceAdress, Y_TARGET_ATTR);
        numberBox.connectWidgetNoMetaData(_yTarget, yTargetKey);
        //Connect xWarnCenter
        TangoKey xWarnCenterKey = new TangoKey();
        TangoKeyTool.registerAttribute(xWarnCenterKey, bptDeviceAdress, X_WARN_ZONE_ATTR);
        numberBox.connectWidgetNoMetaData(_xWarnCenter, xWarnCenterKey);
        //Connect yWarnCenter
        TangoKey yWarnCenterKey = new TangoKey();
        TangoKeyTool.registerAttribute(yWarnCenterKey, bptDeviceAdress, Y_WARN_ZONE_ATTR);
        numberBox.connectWidgetNoMetaData(_yWarnCenter, yWarnCenterKey);
        //Connect warnRadius
        TangoKey warnRadiusKey = new TangoKey();
        TangoKeyTool.registerAttribute(warnRadiusKey, bptDeviceAdress, WARN_ZONE_RADIUS_ATTR);
        numberBox.connectWidgetNoMetaData(_warnRadius, warnRadiusKey);
        //Connect xCentroid
        TangoKey xCentroidKey = new TangoKey();
        TangoKeyTool.registerAttribute(xCentroidKey, bptDeviceAdress, BEAM_X_CENTROID);
        boxStr.connectWidgetNoMetaData(_xCentroid, xCentroidKey);
        //Connect yCentroid
        TangoKey yCentroidKey = new TangoKey();
        TangoKeyTool.registerAttribute(yCentroidKey, bptDeviceAdress, BEAM_Y_CENTROID);
        boxStr.connectWidgetNoMetaData(_yCentroid, yCentroidKey);
        //Connect lockMode
        TangoKey lockModeKey = new TangoKey();
        TangoKeyTool.registerAttribute(lockModeKey, bptDeviceAdress, LOCK_MODE);
        booleanBox.connectWidgetNoMetaData(_lockModeStatus, lockModeKey);
	}
	
	/****************************************************************
	 *  initGui() 
	 *  
	 *  To user interface
	 * **************************************************************/
	private void initGui(){
        _mainFrame = new JFrame(ControlPanel.class.getSimpleName());
        _mainFrame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        _mainFrame.setLayout(new GridLayout(0,2));
        _mainFrame.setBackground(Color.black);
        _mainFrame.add(initLeftPanel());        
        _mainFrame.add(initRightPanel());
        _mainFrame.setSize(new Dimension(1500, 950));
        _mainFrame.setLocationRelativeTo(null);
        _mainFrame.setVisible(true);
	}
	/****************************************************************
	 *  initRightPanel() 
	 *  
	 *  To init right panel
	 * **************************************************************/
	public JPanel initRightPanel(){
		//IMAGE PANEL INIT
		JPanel rightPanel = new JPanel(new GridLayout(4,0));
		Border imagePanBord = BorderFactory.createTitledBorder("Beam Control" );
		
		//BEAM TARGET DEFINITION		
		JPanel centroidPanel = new JPanel(new GridLayout(0,2));
		centroidPanel.setBorder(BorderFactory.createTitledBorder("BEAM CENTROID STATE" ));
		//Current Beam 
		JPanel currentCentroidPanel = new JPanel(new GridLayout(2,2));	
		currentCentroidPanel.setBorder(BorderFactory.createTitledBorder("CENTROIDS"));
		Label xcentroidLab = new Label();
		xcentroidLab.setText("- " + _xAlias +" Current Centroid ");
		Label ycentroidLab = new Label();
		ycentroidLab.setText("- " + _yAlias +" Current Centroid ");
		currentCentroidPanel.add(xcentroidLab);
		currentCentroidPanel.add(ycentroidLab);
		currentCentroidPanel.add(_xCentroid);
		currentCentroidPanel.add(_yCentroid);
		
		_detectorPanel.add(currentCentroidPanel);
		
		//Targeted beam
		JPanel targetCentroidPanel = new JPanel(new GridLayout(2,2));
		targetCentroidPanel.setBorder(BorderFactory.createTitledBorder("TARGETS"));
		//Target labels definitions
		Label xTargetLab = new Label();
		xTargetLab.setText("- " + _xAlias +" Current Target ");
		Label yTargetLab = new Label();
		yTargetLab.setText("- " + _yAlias +" Current Target ");
		targetCentroidPanel.add(xTargetLab);
		targetCentroidPanel.add(yTargetLab);
		JPanel xtargetPanel = new JPanel(new BorderLayout());
		//targetCentroidPanel.add(_xTarget);
		xtargetPanel.add(_xTarget);
		_xTarget.setPreferredSize(xtargetPanel.getSize());
		targetCentroidPanel.add(xtargetPanel);
		targetCentroidPanel.add(_yTarget);
		//Adding to centroid panel
		centroidPanel.add(_detectorPanel);
		centroidPanel.add(targetCentroidPanel);
		
		//WARNING ZONE DEFINITION
		JPanel warningZonePanel = new JPanel(new GridLayout(3,0));
		warningZonePanel.setBorder(BorderFactory.createTitledBorder("WARNING ZONE DEFINITION" ));
		//labels definitions
		Label warnXLab = new Label();
		warnXLab.setText(_xAlias +" Warning zone center");
		Label warnYLab = new Label();
		warnYLab.setText(_yAlias +" Warning zone center");
		Label warnRadiusLab = new Label();
		warnRadiusLab.setText("Warning zone radius");
		//Panel construction
		JPanel warningZoneXPanel = new JPanel(new GridLayout(0,2));
		warningZoneXPanel.setBorder(BorderFactory.createMatteBorder(1, 1, 1, 1, Color.BLACK));
		warningZoneXPanel.add(warnXLab);
		warningZoneXPanel.add(_xWarnCenter);
		JPanel warningZoneYPanel = new JPanel(new GridLayout(0,2));
		warningZoneYPanel.setBorder(BorderFactory.createMatteBorder(0, 1, 0, 1, Color.BLACK));
		warningZoneYPanel.add(warnYLab);
		warningZoneYPanel.add(_yWarnCenter);
		JPanel warningZoneRadPanel = new JPanel(new GridLayout(0,2));
		warningZoneRadPanel.setBorder(BorderFactory.createMatteBorder(1, 1, 1, 1, Color.BLACK));
		warningZoneRadPanel.add(warnRadiusLab);
		warningZoneRadPanel.add(_warnRadius);
		warningZonePanel.add(warningZoneXPanel);
		warningZonePanel.add(warningZoneYPanel);
		warningZonePanel.add(warningZoneRadPanel);
		
		//BPT SATE/STATUS DEFINITION
		JPanel bptStatusPanel = new JPanel(new GridLayout(0,2));
		Border bptStatusBrd = BorderFactory.createTitledBorder("Beam Position Tracking device status" );
		bptStatusPanel.setBorder(bptStatusBrd);
		JPanel bptStatePanel = new JPanel(new GridLayout(2,0));
		bptStatePanel.add(_bptState);
		
		if (_lockModeStatus.getLabel().equals("TRUE"))
			lockStatus.setText(DEVICE_LOCK_MODE_STR);
		else
			lockStatus.setText(DEVICE_DYN_MODE_STR);
		bptStatePanel.add(lockStatus);
		bptStatusPanel.add(bptStatePanel);
		bptStatusPanel.add(_bptStatus);
		
		//AS SATE/STATUS DEFINITION
		JPanel asStatusPanel = new JPanel(new GridLayout(0,2));
		Border asStatusBrd = BorderFactory.createTitledBorder("Actuator system device status" );
		asStatusPanel.setBorder(asStatusBrd);
		asStatusPanel.add(_asState);
		asStatusPanel.add(_asStatus);
	
		rightPanel.setBorder(imagePanBord);
		rightPanel.add(centroidPanel);
		rightPanel.add(warningZonePanel);
		rightPanel.add(bptStatusPanel);
		rightPanel.add(asStatusPanel);
		
		return rightPanel;
	}
	
	/****************************************************************
	 *  initLeftPanel() 
	 *  
	 *  To init left panel
	 * **************************************************************/
	public JPanel initLeftPanel(){
		//IMAGE PANEL INIT
		JPanel leftPanel = new JPanel(new BorderLayout());
		_viewer.setAllowBeamPositionByClick(true);
		_viewer.setAutoZoom(true);
		_viewer.setAutoBestFit(true);
		Border imgBrd = BorderFactory.createTitledBorder("Thresholded Image" );
		_viewer.setBorder(imgBrd);
		//CMD PANNEL INIT
		JPanel cmdPanel = new JPanel(new GridLayout(5,0));
		//state panel
		JPanel xStatePanel = new JPanel(new GridLayout(0,3));
		JPanel yStatePanel = new JPanel(new GridLayout(0,3));
		_isXAxisCalibrated.setFalseLabel("Axis Not Calibrated");
		_isXAxisCalibrated.setTrueLabel("Axis Calibrated");
		_isXAxisCalibrated.setBackground(Color.lightGray);
		_isYAxisCalibrated.setFalseLabel("Axis Not Calibrated");
		_isYAxisCalibrated.setTrueLabel("Axis Calibrated");
		_isYAxisCalibrated.setBackground(Color.lightGray);
		//X PANNEL
		xStatePanel.add(_xAxisState);
		xStatePanel.add(_xAxisPosition);
		xStatePanel.add(_isXAxisCalibrated);
		//Y PANNEL
		yStatePanel.add(_yAxisState);
		yStatePanel.add(_yAxisPosition);
		yStatePanel.add(_isYAxisCalibrated);		
	    Border xBrd = BorderFactory.createTitledBorder(_xAlias +" AXIS STATUS" );
	    xStatePanel.setBorder(xBrd);
	    Border yBrd = BorderFactory.createTitledBorder(_yAlias +" AXIS STATUS" );
	    yStatePanel.setBorder(yBrd);
		
		cmdPanel.add(xStatePanel);
		cmdPanel.add(yStatePanel);
		cmdPanel.add(_startBTButton);
		cmdPanel.add(_stopBTButton);
		cmdPanel.add(_configPanelButton);

		Border imagePanBord = BorderFactory.createTitledBorder("Beam Position" );
		leftPanel.setBorder(imagePanBord);
		leftPanel.add(_viewer, BorderLayout.CENTER);
		leftPanel.add(cmdPanel,BorderLayout.SOUTH);
		
		return leftPanel;
	}
	/****************************************************************
	 *  run() 
	 *  
	 *  
	 * **************************************************************/
	public void run() {
		while (!Thread.currentThread().isInterrupted()) {
			try {
				Thread.sleep(100);
				// To update interface
				refreshInterface();
			} catch (InterruptedException e) {
			}
		}
	}
	/****************************************************************
	 *  showControlPanel() 
	 *  
	 *  
	 * **************************************************************/
	public void showControlPanel(){
		_mainFrame.setVisible(true);
	}
	/****************************************************************
	 *  hideControlPanel() 
	 *  
	 *  
	 * **************************************************************/
	public void hideControlPanel(){
		_mainFrame.setVisible(false);
	}
}
