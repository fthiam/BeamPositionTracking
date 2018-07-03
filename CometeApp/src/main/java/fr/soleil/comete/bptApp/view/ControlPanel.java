package fr.soleil.comete.bptApp.view;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.concurrent.TimeUnit;

import javax.swing.BorderFactory;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.WindowConstants;
import javax.swing.border.Border;

import fr.soleil.comete.bptApp.cometeWrapper.TangoConnection;
import fr.soleil.comete.definition.widget.util.CometeColor;
import fr.soleil.comete.definition.widget.util.Gradient;
import fr.soleil.comete.swing.IconButton;
import fr.soleil.comete.swing.ImageViewer;
import fr.soleil.comete.swing.Label;
import fr.soleil.comete.swing.StringButton;
import fr.soleil.comete.swing.TextArea;
import fr.soleil.comete.swing.WheelSwitch;

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
    private static final String ORIGINAL_IMAGE = "originalImage";
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
    private static final String X_THRESHOLD_ATTR = "xAxisRegulationThreshold";
    private static final String Y_THRESHOLD_ATTR = "yAxisRegulationThreshold";
    private static final String COLD_X_THRESHOLD = "UI_xColdThreshold";
    private static final String HOT_X_THRESHOLD = "UI_xHotThreshold";
    private static final String COLD_Y_THRESHOLD = "UI_yColdThreshold";
    private static final String HOT_Y_THRESHOLD = "UI_yHotThreshold";
    private static final String DEVICE_LOCK_MODE_STR = "Static positionning mode!";
    private static final String DEVICE_DYN_MODE_STR = "Dynamic positionning mode!";
    private static final String IMAGE_SENSOR_TYPE = "Lima"; //TO change : plugin name 
        
    String _bptDeviceAdress;
    boolean _specificPropertiesInitialized;
	boolean _bptReadyToOperate;
	boolean _bptRunning;	
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
	StringButton _startBTButtonCMD;
	StringButton _stopBTButtonCMD;
	StringButton _trackingButton;
	Label _xAxisState;
	Label _xAxisPosition;
	IconButton _isXAxisCalibrated;
	Label _yAxisState;
	Label _yAxisPosition;
	IconButton _isYAxisCalibrated;
	
	//COMPONENTS - RIGHT PANEL
	JPanel _detectorPanel = new JPanel(new GridLayout(2,0));
	Label _bptState;
	TextArea _bptStatus;
	Label _asState;
	TextArea _asStatus;
	WheelSwitch _xTarget;
	WheelSwitch _yTarget;
	WheelSwitch _xWarnCenter;
	WheelSwitch _yWarnCenter;
	WheelSwitch _warnRadius;
	Label _xCentroid;
	Label _yCentroid;
	IconButton _lockModeStatus;
	//Specific Lima plugin 

	IconButton _originalImage;
	WheelSwitch _percentageDetection;
	//axes thresholds
	Label _currentThresholdMode;
	WheelSwitch _currentXThreshold;
	WheelSwitch _currentYThreshold;
	WheelSwitch _coldXThreshold;
	WheelSwitch _hotXThreshold;
	WheelSwitch _coldYThreshold;
	WheelSwitch _hotYThreshold;
	boolean _coldBeamLineMode;
	boolean _hotBeamLineMode;
	StringButton _switchBeamLineThresholdMode;

    String _currentImageMode;
	Label lockStatus;
	boolean _lockMode;
	TangoConnection _tangoConnection = new TangoConnection();
	
	boolean _firstImageSizeFit;
	/****************************************************************
	 *  ControlPanel (String bptDeviceAdress, String asDeviceAdress, ConfigPanel configPanel, String xAlias, String yAlias)
	 *  
	 *  To launch control panel
	 * **************************************************************/
	public ControlPanel (String bptDeviceAdress, String asDeviceAdress, ConfigPanel configPanel, String xAlias, String yAlias){
		_bptDeviceAdress = bptDeviceAdress;
		_configPanel = configPanel;
		_bptReadyToOperate = false;
		_specificPropertiesInitialized = false;
		_firstImageSizeFit = false;
		_bptRunning = false;
		_coldBeamLineMode = false;
		_hotBeamLineMode = false;
		_lockMode = true;
		_xAlias = xAlias;
		_yAlias = yAlias;
		_xOldFrameDim = 0;
		_yOldFrameDim = 0;
		_configPanel.hideConfigPanel();
		_currentImageMode = "Thresholded Image";
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
	 *  setHotBeamLineMode ()
	 *  
	 * **************************************************************/
	private void setHotBeamLineMode(){
		_currentXThreshold.setValue(_hotXThreshold.getValue(), true, true);
		_currentYThreshold.setValue(_hotYThreshold.getValue(), true, true);

		_switchBeamLineThresholdMode.setText("Set Cold mode");
	}
	/****************************************************************
	 *  setColdBeamLineMode ()
	 *  
	 * **************************************************************/
	private void setColdBeamLineMode(){
		_currentXThreshold.setValue(_coldXThreshold.getValue(), true, true);
		_currentYThreshold.setValue(_coldYThreshold.getValue(), true, true);

		_switchBeamLineThresholdMode.setText("Set Cold mode");
	}
	/****************************************************************
	 *  initSpecificProperties ()
	 *  
	 *  To init specific properties
	 * **************************************************************/
	public void initSpecificProperties(){
		//If sensor = image type
		if(_sensorType.getText().equals(IMAGE_SENSOR_TYPE)){
			//Threshold percentage to apply on src image
	        _tangoConnection.connectAttribute(_tangoConnection.numberType, _bptDeviceAdress, PERCENTAGE_DETECTION, _percentageDetection, false);
	        Label thresholdLabel = new Label();
	        thresholdLabel.setText("- Threshold (%) :");
	        JPanel thresholdPanel = new JPanel(new GridLayout(2, 1));
	        thresholdPanel.add(thresholdLabel);
	        thresholdPanel.setBorder(BorderFactory.createTitledBorder("IMAGE THRESHOLD" ));
	        thresholdPanel.add(_percentageDetection);
			//original image option button
			_tangoConnection.connectAttribute(_tangoConnection.booleanType, _bptDeviceAdress,ORIGINAL_IMAGE ,_originalImage, false); 
	        thresholdPanel.add(_originalImage);
			_detectorPanel.add(thresholdPanel);
			_specificPropertiesInitialized = true;
		}
		//else if (another sensor...)
			//_specificPropertiesInitialized = true;
		
	}
	/****************************************************************
	 *  refreshInterface ()
	 *  
	 *  To refresh interface 
	 * **************************************************************/
	private void refreshInterface(){

		refreshBPTState();
		refreshApplicationCommands();
		refreshThresholdMode();
		if(!_specificPropertiesInitialized)
			initSpecificProperties();
		if (!_lockMode)
			getNewSelectedTargetOnImage();
		updateTargetDraw();
		updateImageGradient();
	}
	/****************************************************************
	 *  updateImageGradient ()
	 *  
	 *  Depends on which image we're using : Original or thresholded
	 * **************************************************************/
	private void updateImageGradient(){
		String previousImageMode = _currentImageMode;
		_currentImageMode = _originalImage.getText();
		if (!previousImageMode.equals(_currentImageMode)){
			if (_originalImage.getText().equals("Original Image")){
				Gradient gradient = new Gradient();
		        gradient.buildRainbowGradient();
		        _viewer.setGradient(gradient);
		        _viewer.setBeamColor(Color.BLACK);
			}
			else{
				Gradient gradient = new Gradient();
		        gradient.buildMonochromeGradient();
		        _viewer.setGradient(gradient);
		        _viewer.setBeamColor(Color.RED);
			}
		}		
	}
	/****************************************************************
	 *  refreshThresholdMode ()
	 *  
	 *  Is beam line in Hot/Cold mode?
	 * **************************************************************/
	private void refreshThresholdMode(){
		
		//Beam line is in cold mode
		if ((_coldXThreshold.getValue() == _currentXThreshold.getValue()) &&
			(_coldYThreshold.getValue() == _currentYThreshold.getValue())) {
			_currentThresholdMode.setText("Beam line is currently in Cold mode");
			_switchBeamLineThresholdMode.setText("Set Hot mode");
			_coldBeamLineMode = true;
			_hotBeamLineMode = false;
		}
		//Beam line is in hot mode
		else if ((_hotXThreshold.getValue() == _currentXThreshold.getValue()) &&
				(_hotYThreshold.getValue() == _currentYThreshold.getValue())) {
			_currentThresholdMode.setText("Beam line is currently in Hot mode");
			_switchBeamLineThresholdMode.setText("Set Cold mode");
			_hotBeamLineMode = true;
			_coldBeamLineMode = false;
		}
		//Beam line is in specific mode
		else{
			_currentThresholdMode.setText("Beam line is currently in specific mode");
			_switchBeamLineThresholdMode.setText("Set Cold mode");
			_hotBeamLineMode = false;
			_coldBeamLineMode = false;
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
			_bptRunning = false;
		}
		else if (bptStateStr.equals("RUNNING")){
			_bptReadyToOperate = false;
			_bptRunning = true;
		}
		else {
			_bptReadyToOperate = false;
			_bptRunning = false;
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
		if (_bptReadyToOperate && !_bptRunning){
			_trackingButton.setVisible(true);
			_trackingButton.setText("Start Beam Tracking");
		}else if (_bptRunning){
			_trackingButton.setVisible(true);
			_trackingButton.setText("Stop Beam Tracking");
		}else{
			_trackingButton.setVisible(false);
		}
	}
	/****************************************************************
	 *  initComponents() 
	 *  
	 *  To init components
	 * **************************************************************/
	private void initComponents(){
		//Image component
		_viewer = new ImageViewer(){
            private static final long serialVersionUID = 8555814457975234626L;

            @Override
            protected void initSelectionMenu() {
                super.initSelectionMenu();
                selectionMenu.add(containerFactory.createMenuItem(getAction(MODE_ANGLE_ACTION)));
            }
        };
        _viewer.setAlwaysFitMaxSize(true);
        Gradient gradient = new Gradient();
        gradient.buildMonochromeGradient();
       
        _viewer.setGradient(gradient);
        _viewer.setBeamColor(Color.RED);
        //Control access
        _configPanelButton = new StringButton();
        _configPanelButton.setText("BPT Configuration");
        _configPanelButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				_configPanel.showConfigPanel();
				hideControlPanel();
			}
        });
        //hot beam line 
        _switchBeamLineThresholdMode = new StringButton();
        _switchBeamLineThresholdMode.setText("Set Cold mode");
        _switchBeamLineThresholdMode.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				if (_hotBeamLineMode)
					setColdBeamLineMode();
				else if (_coldBeamLineMode)
					setHotBeamLineMode();
				else 
					setColdBeamLineMode();
			}
        });
        
        //Starts and stops tracking
        _trackingButton = new StringButton();
        _trackingButton.setText("Start Beam Tracking");
        _trackingButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				if (_bptRunning)
					_stopBTButtonCMD.doClick();
				else if (!_bptRunning && _bptReadyToOperate)
					_startBTButtonCMD.doClick();
			}
        });
        

		Font boldFontTarget = new Font("Verdana", Font.BOLD, 20);
		Font boldFontCentroid = new Font("Verdana", Font.BOLD, 15);
		
        _sensorType = new Label();
        //BPT State/Status
        _bptState = new Label();
        _bptStatus = new TextArea();
        //AS State/Status
    	_asState = new Label();
    	_asStatus = new TextArea();
        //start beam tracking button 
		_startBTButtonCMD = new StringButton();
        //stop beam tracking button
        _stopBTButtonCMD = new StringButton();
        //States
        _xAxisState = new Label();
		_yAxisState = new Label();
		//Axes positions 
		_xAxisPosition = new Label();
		_yAxisPosition = new Label();
		//Axes calibration state
		_isXAxisCalibrated = new IconButton();
		_isYAxisCalibrated = new IconButton();
		//Percentage detection
		_percentageDetection = new WheelSwitch();
		_originalImage = new IconButton();
		
		_originalImage.setTrueIconId(0);
		_originalImage.setFalseIconId(0);
		_originalImage.setTrueLabel("Original Image");
		_originalImage.setFalseLabel("Thresholded Image");
		//Beam targets
		_xTarget = new WheelSwitch();
		_xTarget.setFont(boldFontTarget);
		_yTarget = new WheelSwitch();
		_yTarget.setFont(boldFontTarget);
		//Warning zone
		_xWarnCenter = new WheelSwitch();
		_yWarnCenter = new WheelSwitch();
		_warnRadius = new WheelSwitch();
		//Beam Centroid
		_xCentroid = new Label();
		_xCentroid.setFont(boldFontCentroid);
		_xCentroid.setCometeBackground(CometeColor.CYAN);
		_yCentroid = new Label();

		_yCentroid.setCometeBackground(CometeColor.CYAN);
		_yCentroid.setFont(boldFontCentroid);
		//lock mode
		_lockModeStatus = new IconButton();
		lockStatus = new Label();
		//axes thresholds
		_currentXThreshold = new WheelSwitch();
		_currentYThreshold = new WheelSwitch();
		_coldXThreshold = new WheelSwitch();
		_hotXThreshold = new WheelSwitch();
		_coldYThreshold = new WheelSwitch();
		_hotYThreshold = new WheelSwitch();
		_currentThresholdMode = new Label();
	}
	
	/****************************************************************
	 *  initTangoConnexions() 
	 *  
	 *  To init tango connexions
	 * **************************************************************/
	private void initTangoConnexions(String bptDeviceAdress, String asDeviceAdress){
		  			             
        // Image connection
        _tangoConnection.connectAttribute( bptDeviceAdress, IMAGE_ATTR, _viewer);
        //Sensor type : used to initialized specific stuff in UI
        _tangoConnection.connectProperty(_tangoConnection.stringType,bptDeviceAdress,SENSOR_TYPE_PROP,_sensorType);
        //Connect BPT state
        _tangoConnection.connectAttribute(_tangoConnection.stringType,bptDeviceAdress+ "/state" ,"",_bptState, true);
        //Connect BPT status
        _tangoConnection.connectAttribute(_tangoConnection.stringType, bptDeviceAdress, DEVICE_STATUS, _bptStatus, false);
        //Connect AS state
        _tangoConnection.connectAttribute(_tangoConnection.stringType,asDeviceAdress+ "/state" ,"",_asState, true);
        //Connect AS status
        _tangoConnection.connectAttribute(_tangoConnection.stringType, asDeviceAdress, DEVICE_STATUS, _asStatus, false);
        //Connect BeamTracking commands
        _tangoConnection.connectCommand(_tangoConnection.stringType, bptDeviceAdress, START_BEAM_TRACKING_CMD, _startBTButtonCMD, false);
        _tangoConnection.connectCommand(_tangoConnection.stringType, bptDeviceAdress, STOP_BEAM_TRACKING_CMD, _stopBTButtonCMD, false);
        //Connect axes states Labels
        _tangoConnection.connectAttribute(_tangoConnection.stringType,asDeviceAdress ,"xState",_xAxisState, true);
        _tangoConnection.connectAttribute(_tangoConnection.stringType,asDeviceAdress ,"yState",_yAxisState, true);
        //Connect axes positions Labels
        _tangoConnection.connectAttribute(_tangoConnection.stringType, asDeviceAdress, X_POSITION_ATTR, _xAxisPosition, false);
        _tangoConnection.connectAttribute(_tangoConnection.stringType, asDeviceAdress, Y_POSITION_ATTR, _yAxisPosition, false);
        //Connect axes calibration states
        _tangoConnection.connectAttribute(_tangoConnection.booleanType, asDeviceAdress, X_AXIS_CAL_ATTR, _isXAxisCalibrated, false);
        _tangoConnection.connectAttribute(_tangoConnection.booleanType, asDeviceAdress, Y_AXIS_CAL_ATTR, _isYAxisCalibrated, false);
        //Connect targets  
        _tangoConnection.connectAttribute(_tangoConnection.numberType, bptDeviceAdress, X_TARGET_ATTR,_xTarget, false);
        _tangoConnection.connectAttribute(_tangoConnection.numberType, bptDeviceAdress, Y_TARGET_ATTR,_yTarget, false);
        //Connect warningZone
        _tangoConnection.connectAttribute(_tangoConnection.numberType, bptDeviceAdress, X_WARN_ZONE_ATTR,_xWarnCenter, false);
        _tangoConnection.connectAttribute(_tangoConnection.numberType, bptDeviceAdress, Y_WARN_ZONE_ATTR,_yWarnCenter, false);
        _tangoConnection.connectAttribute(_tangoConnection.numberType, bptDeviceAdress, WARN_ZONE_RADIUS_ATTR,_warnRadius, false);
        //Connect centroids
        _tangoConnection.connectAttribute(_tangoConnection.stringType,bptDeviceAdress ,BEAM_X_CENTROID,_xCentroid, false);
        _tangoConnection.connectAttribute(_tangoConnection.stringType,bptDeviceAdress ,BEAM_Y_CENTROID,_yCentroid, false);
        //Connect lockMode
        _tangoConnection.connectAttribute(_tangoConnection.booleanType, bptDeviceAdress, LOCK_MODE, _lockModeStatus, false);
        //Connect thresholds
        _tangoConnection.connectAttribute(_tangoConnection.numberType,bptDeviceAdress ,X_THRESHOLD_ATTR,_currentXThreshold, false);
        _tangoConnection.connectAttribute(_tangoConnection.numberType,bptDeviceAdress ,Y_THRESHOLD_ATTR,_currentYThreshold, false);
        _tangoConnection.connectProperty(_tangoConnection.numberType,bptDeviceAdress,COLD_X_THRESHOLD,_coldXThreshold);
        _tangoConnection.connectProperty(_tangoConnection.numberType,bptDeviceAdress,COLD_Y_THRESHOLD,_coldYThreshold);
        _tangoConnection.connectProperty(_tangoConnection.numberType,bptDeviceAdress,HOT_X_THRESHOLD,_hotXThreshold);
        _tangoConnection.connectProperty(_tangoConnection.numberType,bptDeviceAdress,HOT_Y_THRESHOLD,_hotYThreshold);
        
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
		xtargetPanel.add(_xTarget);
		_xTarget.setPreferredSize(xtargetPanel.getSize());
		targetCentroidPanel.add(xtargetPanel);
		targetCentroidPanel.add(_yTarget);
		//Adding to centroid panel
		centroidPanel.add(_detectorPanel);
		centroidPanel.add(targetCentroidPanel);
		
		//ASSERVISSEMENT LIMITS
		JPanel asservissementLimits = new JPanel(new GridLayout(0,2));
		//WARNING ZONE DEFINITION
		JPanel warningZonePanel = new JPanel(new GridLayout(3,0));
		warningZonePanel.setBorder(BorderFactory.createTitledBorder("WARNING ZONE : " ));
		//Panel construction
		JPanel warningZoneXPanel = new JPanel(new GridLayout(0,1));
		warningZoneXPanel.setBorder(BorderFactory.createTitledBorder(_xAlias +" center"));
		warningZoneXPanel.add(_xWarnCenter);
		JPanel warningZoneYPanel = new JPanel(new GridLayout(0,1));
		warningZoneYPanel.setBorder(BorderFactory.createTitledBorder(_yAlias +" center"));
		warningZoneYPanel.add(_yWarnCenter);
		JPanel warningZoneRadPanel = new JPanel(new GridLayout(0,1));
		warningZoneRadPanel.setBorder(BorderFactory.createTitledBorder("Radius"));
		warningZoneRadPanel.add(_warnRadius);
		warningZonePanel.add(warningZoneXPanel);
		warningZonePanel.add(warningZoneYPanel);
		warningZonePanel.add(warningZoneRadPanel);
		asservissementLimits.setBorder(BorderFactory.createTitledBorder("ASSERVISSEMENT LIMITS" ));
		asservissementLimits.add(warningZonePanel);
		JPanel thresholdsPannel = new JPanel(new GridLayout(2,0));
		thresholdsPannel.setBorder(BorderFactory.createTitledBorder("BEAM LINE THRESHOLD STATUS" ));
		thresholdsPannel.add(_currentThresholdMode);
		thresholdsPannel.add(_switchBeamLineThresholdMode);
		asservissementLimits.add(thresholdsPannel);
		
		
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
		rightPanel.add(asservissementLimits);
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
		JPanel cmdPanel = new JPanel(new GridLayout(4,0));
		//state panel
		JPanel xStatePanel = new JPanel(new GridLayout(0,3));
		JPanel yStatePanel = new JPanel(new GridLayout(0,3));
		_isXAxisCalibrated.setFalseLabel("Axis Not Calibrated");
		_isXAxisCalibrated.setTrueLabel("Axis Calibrated");
		_isXAxisCalibrated.setBackground(Color.lightGray);
		_isYAxisCalibrated.setFalseLabel("Axis Not Calibrated");
		_isYAxisCalibrated.setTrueLabel("Axis Calibrated");
		_isYAxisCalibrated.setBackground(Color.lightGray);
		//Axes states
		xStatePanel.add(_xAxisState);
		xStatePanel.add(_xAxisPosition);
		//Label xUnit = new Label();
		//xStatePanel.add(xUnit);
		xStatePanel.add(_isXAxisCalibrated);
		yStatePanel.add(_yAxisState);
		yStatePanel.add(_yAxisPosition);
		//Label yUnit = new Label();
		//yStatePanel.add(yUnit);
		yStatePanel.add(_isYAxisCalibrated);		
	    Border xBrd = BorderFactory.createTitledBorder(_xAlias +" AXIS STATUS" );
	    xStatePanel.setBorder(xBrd);
	    Border yBrd = BorderFactory.createTitledBorder(_yAlias +" AXIS STATUS" );
	    yStatePanel.setBorder(yBrd);
		
		cmdPanel.add(xStatePanel);
		cmdPanel.add(yStatePanel);
		cmdPanel.add(_trackingButton);
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
