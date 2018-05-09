package main.java.fr.soleil.comete.bptApp;
import java.awt.Dimension;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.swing.SwingConstants;

import fr.soleil.comete.swing.Panel;
import main.java.fr.soleil.comete.bptApp.view.ConfigPanel;
import main.java.fr.soleil.comete.bptApp.view.ControlPanel;

public class BeamPositionTracking {

	final static Panel panel = new Panel();
	static ControlPanel controlPanel;
	static ConfigPanel configPanel;
	public static void main(String[] args) {
		configPanel = new ConfigPanel();
	}
	
}
