package fr.soleil.comete.bptApp.view;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Scanner;
import java.util.prefs.BackingStoreException;
import java.util.prefs.Preferences;

import javax.swing.BorderFactory;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingWorker;
import javax.swing.WindowConstants;
import javax.swing.border.Border;
import javax.swing.border.TitledBorder;

import fr.soleil.comete.bptApp.cometeWrapper.TangoConnection;
import fr.soleil.comete.swing.Label;
import fr.soleil.comete.swing.StringButton;
import fr.soleil.lib.project.awt.WindowUtils;
import fr.soleil.lib.project.swing.dialog.ProgressDialog;

public class ConfigPanel implements Runnable  {
	private static final String BPT_DEVICE_CLASS = "BeamPositionTracking";
	private static final String X_PID_PROP = "XPIDDefinition";
	private static final String Y_PID_PROP = "YPIDDefinition";
	private static final String X_MIN_POSITION = "XAxisMinPosition";
	private static final String X_MAX_POSITION = "XAxisMaxPosition";
	private static final String Y_MIN_POSITION = "YAxisMinPosition";
	private static final String Y_MAX_POSITION = "YAxisMaxPosition";
	private static final String X_CALIBRATION_STEP_NUMBER = "CalibrationStepNbXAxis";
	private static final String Y_CALIBRATION_STEP_NUMBER = "CalibrationStepNbYAxis";
	private static final String AS_DEVICE_ADRESS = "ActuatorSystemDeviceAdress";
    private static final String COLD_X_THRESHOLD = "UI_xColdThreshold";
    private static final String HOT_X_THRESHOLD = "UI_xHotThreshold";
    private static final String COLD_Y_THRESHOLD = "UI_yColdThreshold";
    private static final String HOT_Y_THRESHOLD = "UI_yHotThreshold";
    private static final String AXES_ALIASES = "AxesAliases";
	boolean _bptDeviceReadyToConnect;
	boolean _bptDeviceAddressHasChange;
	String _bptDeviceAdress;
	JFrame _mainFrame;
	ConfigPanel _configPanel;
	static ControlPanel _controlPanel;
	StringButton _controlRedirection;
	private ProgressDialog progressDialog;
	

    Label _targetDeviceClassLabel = new Label();
	
	//Right panel
	JPanel _rightPanel;
	Label _asAdressLabel;
	JTextField _bptDeviceAdressTextField;
	StringButton _connectBPT;
	Label _bptState;
	Label _asState;
	Label _bptConnectionStatus;
	
	JPanel _asDescPanel;
	//Left panel
	JPanel _leftPanel;
	Label _bptMode;
	Label _xPid;
	Label _xMinPosition;
	Label _xMaxPosition;
	Label _xCalibrationStepNumber;
	Label _yPid;
	Label _yMinPosition;
	Label _yMaxPosition;
	Label _yCalibrationStepNumber;
	Label _axesAliases;
	String _xAlias;
	String _yAlias;
	
	//PID info
	boolean _pidInfoInitDone;
	//AS device init
	boolean _asDeviceInitDone;
	Label _isXPidInUse;
	Label _xPCoef;
	Label _xICoef;
	Label _xDCoef;
	Label _isYPidInUse;
	Label _yPCoef;
	Label _yICoef;
	Label _yDCoef;
	
	//specific ui properties
	Label _coldXThreshold;
	Label _hotXThreshold;
	Label _coldYThreshold;
	Label _hotYThreshold;
	
	//ConfigPannel State
	boolean _bptReady;
	boolean _asReady;
	boolean _axesAliasesInit;
	
	TangoConnection _tangoConnection = new TangoConnection();
	/****************************************************************
	 *  ConfigPanel() 
	 *  
	 *
	 * **************************************************************/
	public ConfigPanel(){
		_bptDeviceReadyToConnect = false;
		_bptDeviceAddressHasChange = false;
		_axesAliasesInit = false;
		_asDeviceInitDone = false;
		_pidInfoInitDone = false;
		_bptReady = false;
		_asReady = false;
		_configPanel = this;
		_xAlias = "x";
		_yAlias = "y";
		initComponents();
		initGui();
		run();
		
	}
	/****************************************************************
	 *  refreshInterface() 
	 *  
	 *
	 * **************************************************************/
	public void refreshInterface(){
		//BPT connection is ok
		if (_bptReady && _asReady /*&& _axesAliasesInit*/){
			_controlRedirection.setVisible(true);
			_rightPanel.setVisible(true);
		}
		//BPT connection not ok
		else {
			if(_bptDeviceReadyToConnect)
				if(checkBptDeviceClass()){
					initializeBPTConnection();
					_bptDeviceReadyToConnect = false;
				}
			_controlRedirection.setVisible(false);
			_rightPanel.setVisible(false);
		}
		//If bpt is ready, initialize as
		if(!_asReady && _bptReady){
			_asDescPanel.setVisible(false);
			initASDevice();
		}
		//If as device is ready, print panel
		if(_asReady)
			_asDescPanel.setVisible(true);
		else
			_asDescPanel.setVisible(false);
		
		if (!_pidInfoInitDone && _bptReady)
			_pidInfoInitDone = parsePIDs();
		if(!_axesAliasesInit && _bptReady)
			initAliases();
	}
	/****************************************************************
	 *  initASDevice() 
	 *  
	 *
	 * **************************************************************/
	public void initASDevice(){
		if (!_asAdressLabel.getText().isEmpty())
			connectAs();		
	}
	/****************************************************************
	 *  initComponents() 
	 *  
	 *  To init components
	 * **************************************************************/
	private void initComponents(){
		//Init control "redirection"
		_controlRedirection = new StringButton();
		_controlRedirection.setText("BPT Control");
		_controlRedirection.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				//Object control panel not created yet
				if (_controlPanel == null && !_asAdressLabel.getText().isEmpty()){	
							
					_controlPanel = new ControlPanel(_bptDeviceAdressTextField.getText(),
							 						  _asAdressLabel.getText(),
							 						  _configPanel, 
							 						 _xAlias, _yAlias);
					 //start control panel thread in new thread
					 new Thread(new Runnable() {
					      public void run() {
					    	  _controlPanel.run();
					    	  }				    		  
					      }
				         ).start();
				}
				//If control panel started, then show it
				else {
					if(_bptDeviceAddressHasChange){
						_controlPanel.updateDevices(_bptDeviceAdressTextField.getText(), _asAdressLabel.getText());
						_bptDeviceAddressHasChange = false;
					}
					_controlPanel.showControlPanel();
					hideConfigPanel();
				}
			}
		});
		
		//Devices Address
		_bptDeviceAdressTextField = new JTextField(getUserPreference("bptFavoriteDevice"), 20);

		//States
		_bptState = new Label();
		_asState = new Label();		

		//axes alias 
		_axesAliases = new Label();		

		//Connect BPT button
		_connectBPT = new StringButton();
		_connectBPT.setText("BPT connection");
		_connectBPT.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				connectBpt();
			}
		});
		
		//connection status
		_bptConnectionStatus = new Label();
		//X properties info
		_xPid = new Label();
		_xMinPosition = new Label();
		_xMaxPosition = new Label();
		_xCalibrationStepNumber = new Label();
		//Y properties info
		_yPid = new Label();
		_yMinPosition = new Label();
		_yMaxPosition = new Label();
		_yCalibrationStepNumber = new Label();	
		//PIDs
		_isXPidInUse = new Label();
		_xPCoef = new Label();
		_xICoef = new Label();
		_xDCoef = new Label();
		_isYPidInUse = new Label();
		_yPCoef = new Label();
		_yICoef = new Label();
		_yDCoef = new Label();
		//AS 
		_asDescPanel = new JPanel(new GridLayout(2,0));
		_asAdressLabel = new Label();
		//UI - specifics
		_coldXThreshold = new Label();
		_coldYThreshold = new Label();
		_hotXThreshold = new Label();
		_hotYThreshold = new Label();
		
	}
	/****************************************************************
	 *  connectBpt() 
	 *  
	 *  
	 * **************************************************************/
	public void connectBpt(){
		if (progressDialog==null) {
			progressDialog = new ProgressDialog(WindowUtils.getWindowForComponent(_connectBPT));
			progressDialog.setProgressIndeterminate(true);
		}
		if(!progressDialog.isVisible()) {
			progressDialog.setTitle("Connecting...");
			progressDialog.setMainMessage("Connecting BPT...");
			progressDialog.pack();
			progressDialog.setLocationRelativeTo(_connectBPT);
			progressDialog.setVisible(true);
		}
		
		//check if bpt address has changed => to update control panel 
		if (!_bptDeviceAdressTextField.getText().equals(_bptDeviceAdress))
			_bptDeviceAddressHasChange = true;
	
		_bptReady = false;
		_asReady = false;
		_bptConnectionStatus.setText("No BeamPositionTracking device connected yet !");
		_bptDeviceAdress = _bptDeviceAdressTextField.getText();
		_connectBPT.setEnabled(false);
        SwingWorker<Void, Void> worker = new SwingWorker<Void, Void>() {
			
			@Override
			protected Void doInBackground() throws Exception {
				if(_tangoConnection.connectLabelDeviceClass(_bptDeviceAdress, _targetDeviceClassLabel)){
		        	_bptDeviceReadyToConnect = true;
		        } else {
		        	_bptDeviceReadyToConnect = false;
		        }
				return null;
			}
			@Override
			protected void done() {
				_connectBPT.setEnabled(true);
				if(!_bptDeviceReadyToConnect) {
		    		_bptConnectionStatus.setText("Counldn't connect to distant device"
							+ "\nPlease check device adress !");	
				}
		        progressDialog.setVisible(false);
			}
		};
		setUserPreference ("bptFavoriteDevice", _bptDeviceAdressTextField.getText());
		worker.execute();
	}
	/****************************************************************
	 *  initializeBPTConnection() 
	 *  
	 *  
	 * **************************************************************/
	public void initializeBPTConnection(){
		_tangoConnection.connectProperty(_tangoConnection.stringType, _bptDeviceAdress, X_PID_PROP, _xPid );
		_tangoConnection.connectProperty(_tangoConnection.stringType, _bptDeviceAdress, Y_PID_PROP, _yPid );
		_tangoConnection.connectProperty(_tangoConnection.stringType, _bptDeviceAdress, X_CALIBRATION_STEP_NUMBER, _xCalibrationStepNumber );
		_tangoConnection.connectProperty(_tangoConnection.stringType, _bptDeviceAdress, Y_CALIBRATION_STEP_NUMBER, _yCalibrationStepNumber );
		_tangoConnection.connectProperty(_tangoConnection.stringType, _bptDeviceAdress, AS_DEVICE_ADRESS, _asAdressLabel );
		_tangoConnection.connectProperty(_tangoConnection.stringType, _bptDeviceAdress, AXES_ALIASES, _axesAliases );
		_tangoConnection.connectProperty(_tangoConnection.stringType, _bptDeviceAdress, AXES_ALIASES, _axesAliases );

		_tangoConnection.connectProperty(_tangoConnection.stringType, _bptDeviceAdress, COLD_X_THRESHOLD, _coldXThreshold );
		_tangoConnection.connectProperty(_tangoConnection.stringType, _bptDeviceAdress, HOT_X_THRESHOLD, _hotXThreshold );
		_tangoConnection.connectProperty(_tangoConnection.stringType, _bptDeviceAdress, COLD_Y_THRESHOLD, _coldYThreshold);
		_tangoConnection.connectProperty(_tangoConnection.stringType, _bptDeviceAdress, HOT_Y_THRESHOLD, _hotYThreshold );
		
		_bptConnectionStatus.setText("Device BeamTrackingPosition : " + _bptDeviceAdress + " is connected !");
		this._bptReady = true;	
	}
	/****************************************************************
	 *  checkBptDeviceClass() 
	 *  
	 *  
	 * **************************************************************/
	public boolean checkBptDeviceClass(){
		String distantDeviceClass = _targetDeviceClassLabel.getText();
    	
    	if(!distantDeviceClass.isEmpty()){
	        if (distantDeviceClass.equals(BPT_DEVICE_CLASS)){
	        	return true;
	        }
	        else{
	        	_bptConnectionStatus.setText("Device adress incorrect"
											+" , this is not a BeamPositionTracking device!");
	        	_bptDeviceReadyToConnect = false;
	        	return false;
	        }
    	}
    	return false;
	}
	/****************************************************************
	 *  initAliases() 
	 *  
	 *  To init axes aliases
	 * **************************************************************/
	public void initAliases(){
		if (!_axesAliases.getText().equals("")){
			_axesAliasesInit = true;
			String [] aliases = _axesAliases.getText().split("\n");
			_xAlias = aliases[0];
			_yAlias = aliases[1];
		}
	}
	/****************************************************************
	 *  ParsePIDs(Label xPid, Label yPid) 
	 *  
	 *  
	 * **************************************************************/
	public boolean parsePIDs(){
		boolean xPidInfoInitDone = false;
		boolean yPidInfoInitDone = false;
		String xPIDstr = _xPid.getText();
		String yPIDstr = _yPid.getText();

		//Don't know exactly when property will be filled up
		if (!xPIDstr.isEmpty()){
			Scanner s = new Scanner(xPIDstr).useDelimiter("::");
			_isXPidInUse.setText(s.next());
			_xPCoef.setText(s.next());
			_xICoef.setText(s.next());
			_xDCoef.setText(s.next());
			xPidInfoInitDone = true;
		}
		//Don't know exactly when property will be filled up
		if (!yPIDstr.isEmpty()){
			Scanner s = new Scanner(yPIDstr).useDelimiter("::");
			_isYPidInUse.setText(s.next());
			_yPCoef.setText(s.next());
			_yICoef.setText(s.next());
			_yDCoef.setText(s.next());
			yPidInfoInitDone = true;
		}
		
		return (xPidInfoInitDone && yPidInfoInitDone);
	}
	/****************************************************************
	 *  connectAs() 
	 *  
	 *  
	 * **************************************************************/
	public void connectAs(){
		String asDeviceAdress = _asAdressLabel.getText();
		Label tmpLabel = new Label();
		if(_tangoConnection.connectLabelDeviceClass(asDeviceAdress, tmpLabel)){
			_tangoConnection.connectAttribute(_tangoConnection.stringType, asDeviceAdress+ "/state", "", _asState, true);
			//INIT PROPERTIES VALUES  !
			_tangoConnection.connectProperty(_tangoConnection.stringType, asDeviceAdress, X_MIN_POSITION, _xMinPosition );
			_tangoConnection.connectProperty(_tangoConnection.stringType, asDeviceAdress, X_MAX_POSITION, _xMaxPosition );
			_tangoConnection.connectProperty(_tangoConnection.stringType, asDeviceAdress, Y_MIN_POSITION, _yMinPosition );
			_tangoConnection.connectProperty(_tangoConnection.stringType, asDeviceAdress, Y_MAX_POSITION, _yMaxPosition );
			this._asReady = true;
		}
		else 
			this._asReady = false;
	}
	
	/****************************************************************
	 *  initGui() 
	 *  
	 *  To user interface
	 * **************************************************************/
	private void initGui(){
		
        _mainFrame = new JFrame(ConfigPanel.class.getSimpleName());
        _mainFrame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        _mainFrame.setLayout(new GridLayout(0,2));

        //Init panels
        _leftPanel = initLeftPanel();
        _rightPanel = initRightPanel();
        _mainFrame.add(_leftPanel);        
        _mainFrame.add(_rightPanel);
        
        Dimension appDim = new Dimension(1500, 600);
        _mainFrame.setSize(appDim);
        _mainFrame.setLocationRelativeTo(null);
        _mainFrame.setVisible(true);
	}
	/****************************************************************
	 *  initRightPanel() 
	 *  
	 *  To init right panel
	 * **************************************************************/
	public JPanel initRightPanel(){
		JPanel rightPanel = new JPanel(new GridLayout(4,0));
		Border rightPanelBord = BorderFactory.createTitledBorder("BPT Configuration");
			
		//AXES LIMITS PROPERTIES		
		JPanel axesLimitsPanel = new JPanel(new GridLayout(0,2));
		axesLimitsPanel.setBorder(BorderFactory.createTitledBorder("AXES LIMITS" ));
		//X limits
		JPanel xLimitsPanel = new JPanel(new GridLayout(2,2));	
		xLimitsPanel.setBorder(BorderFactory.createTitledBorder("X Axis" ));
		Label xMinLimLab = new Label();
		xMinLimLab.setText("	Min limit ");
		Label xMaxLimLab = new Label();
		xMaxLimLab.setText("	Max limit ");
		xLimitsPanel.add(xMinLimLab);
		xLimitsPanel.add(_xMinPosition);
		xLimitsPanel.add(xMaxLimLab);
		xLimitsPanel.add(_xMaxPosition);
		//Y limits
		JPanel yLimitsPanel = new JPanel(new GridLayout(2,2));	
		yLimitsPanel.setBorder(BorderFactory.createTitledBorder("Y Axis" ));
		Label yMinLimLab = new Label();
		yMinLimLab.setText("	Min limit ");
		Label yMaxLimLab = new Label();
		yMaxLimLab.setText("	Max limit ");
		yLimitsPanel.add(yMinLimLab);
		yLimitsPanel.add(_yMinPosition);
		yLimitsPanel.add(yMaxLimLab);
		yLimitsPanel.add(_yMaxPosition);
		axesLimitsPanel.add(xLimitsPanel);
		axesLimitsPanel.add(yLimitsPanel);
		
		//PID VALUES
		JPanel axesPIDsPanel = new JPanel(new GridLayout(0,2));
		axesPIDsPanel.setBorder(BorderFactory.createTitledBorder("AXES PIDs" ));

		Label pXCoefLab = new Label();
		pXCoefLab.setText("P coef :");
		Label iXCoefLab = new Label();
		iXCoefLab.setText("I coef :");
		Label dXCoefLab = new Label();
		dXCoefLab.setText("D coef :");
		JPanel xPIDPanel = new JPanel(new GridLayout(0,3));	
		xPIDPanel.setBorder(BorderFactory.createTitledBorder("X PID "));
		
		JPanel xPPanel = new JPanel(new GridLayout(0,2));
		xPPanel.add(pXCoefLab);
		xPPanel.add(_xPCoef);
		JPanel xIPanel = new JPanel(new GridLayout(0,2));
		xIPanel.add(iXCoefLab);
		xIPanel.add(_xICoef);
		JPanel xDPanel = new JPanel(new GridLayout(0,2));
		xDPanel.add(dXCoefLab);
		xDPanel.add(_xDCoef);
		
		xPIDPanel.add(xPPanel);
		xPIDPanel.add(xIPanel);
		xPIDPanel.add(xDPanel);
		//X limits
		Label pYCoefLab = new Label();
		pYCoefLab.setText("P coef :");
		Label iYCoefLab = new Label();
		iYCoefLab.setText("I coef :");
		Label dYCoefLab = new Label();
		dYCoefLab.setText("D coef :");
		JPanel yPIDPanel = new JPanel(new GridLayout(0,3));	
		yPIDPanel.setBorder(BorderFactory.createTitledBorder("Y PID "));
		
		JPanel yPPanel = new JPanel(new GridLayout(0,2));
		yPPanel.add(pYCoefLab);
		yPPanel.add(_yPCoef);
		JPanel yIPanel = new JPanel(new GridLayout(0,2));
		yIPanel.add(iYCoefLab);
		yIPanel.add(_yICoef);
		JPanel yDPanel = new JPanel(new GridLayout(0,2));
		yDPanel.add(dYCoefLab);
		yDPanel.add(_yDCoef);
		
		//Axes thresholds
		JPanel thresholdPanel = new JPanel(new GridLayout(0,2));
		thresholdPanel.setBorder(BorderFactory.createTitledBorder("Axes thresholds configuration (in pixels)"));
		JPanel coldThresholdPanel = new JPanel(new GridLayout(2,2));
		//cold threshold
		coldThresholdPanel.setBorder(BorderFactory.createTitledBorder("Cold beam line"));
		Label coldXLabel = new Label();
		coldXLabel.setText("X axis ");
		coldThresholdPanel.add(coldXLabel);
		coldThresholdPanel.add(_coldXThreshold);
		Label coldYLabel = new Label();
		coldYLabel.setText("Y axis ");
		coldThresholdPanel.add(coldYLabel);
		coldThresholdPanel.add(_coldYThreshold);
		//hot threshold
		JPanel hotThresholdPanel = new JPanel(new GridLayout(2,2));
		hotThresholdPanel.setBorder(BorderFactory.createTitledBorder("Hot beam line"));
		Label hotXLabel = new Label();
		hotXLabel.setText("X axis ");
		hotThresholdPanel.add(hotXLabel);
		hotThresholdPanel.add(_hotXThreshold);
		Label hotYLabel = new Label();
		hotYLabel.setText("Y axis ");
		hotThresholdPanel.add(hotYLabel);
		hotThresholdPanel.add(_hotYThreshold);
		thresholdPanel.add(coldThresholdPanel);
		thresholdPanel.add(hotThresholdPanel);
		
		yPIDPanel.add(yPPanel);
		yPIDPanel.add(yIPanel);
		yPIDPanel.add(yDPanel);
		
		axesPIDsPanel.add(xPIDPanel);
		axesPIDsPanel.add(yPIDPanel);

		rightPanel.add(axesLimitsPanel);
		rightPanel.add(axesPIDsPanel);
		rightPanel.add(thresholdPanel);
		rightPanel.setBorder(rightPanelBord);
		return rightPanel;
	}
	/****************************************************************
	 *  initRightPanel() 
	 *  
	 *  To init right panel
	 * **************************************************************/
	public JPanel initLeftPanel(){
		JPanel leftPanel = new JPanel(new GridLayout(3,0));
		Border leftPanelBord = BorderFactory.createTitledBorder("Devices connections");
		
		//BPT connection
		_bptDeviceAdressTextField.setBorder(new TitledBorder("BPT adress"));
		JPanel deviceBptconnectionStateButton = new JPanel(new GridLayout(0,2));
		deviceBptconnectionStateButton.add(_connectBPT);
		deviceBptconnectionStateButton.add(_bptState);
		JPanel deviceBPTconnection = new JPanel(new GridLayout(3,0));
		deviceBPTconnection.add(_bptDeviceAdressTextField);
		deviceBPTconnection.add(deviceBptconnectionStateButton);
		_bptConnectionStatus.setBorder(new TitledBorder("BeamTrackingPosition device connection state"));

		_bptConnectionStatus.setText("No BeamPositionTracking device connected yet !");
		deviceBPTconnection.add(_bptConnectionStatus);

		_asDescPanel.setBorder(new TitledBorder("Actuator system paired :"));
		_asDescPanel.add(_asAdressLabel);
		_asDescPanel.add(_asState);
		
		leftPanel.add(deviceBPTconnection);
		leftPanel.add(_asDescPanel);
		leftPanel.add(_controlRedirection);
		leftPanel.setBorder(leftPanelBord);
		return leftPanel;
	}
	/****************************************************************
	 *  hideConfigPanel() 
	 *  
	 *  
	 * **************************************************************/
	public void hideConfigPanel(){
		_mainFrame.setVisible(false);
	}
	/****************************************************************
	 *  showConfigPanel() 
	 *  
	 *  
	 * **************************************************************/
	public void showConfigPanel(){
		_mainFrame.setVisible(true);
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
				// To interface
				refreshInterface();
			} catch (InterruptedException e) {
			}
		}
	}
	/****************************************************************
	 *  getUserPreference(String prefName) 
	 *  
	 *  
	 * **************************************************************/
	private String getUserPreference(String prefName) {
		Preferences prefs = Preferences.userRoot().node(this.getClass().getName());
		return prefs.get(prefName, "Enter/Here/BeamPositionDeviceAdress");
	}
	/****************************************************************
	 *  setUserPreference(String prefName, String value)
	 *  
	 *  
	 * **************************************************************/
	private void setUserPreference(String prefName, String value) {
		Preferences prefs = Preferences.userRoot().node(this.getClass().getName());
		prefs.put(prefName, value);
		try {
			prefs.flush();
		} catch (BackingStoreException e) {
			e.printStackTrace();
		}
	}
}
