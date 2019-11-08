package com.sensing.core.coreviewer;

import android.support.v4.app.Fragment;

public abstract class GraphFragment extends Fragment implements GraphInterface {
    public abstract void zoomIn();

    public abstract void zoomOut();

    public abstract void takeSnapshot();

    public abstract void setVerticalAxisLabel(String title);

    public abstract void setmMarkerResetInterval(int interval);

    public abstract void setAutoscale(boolean isOn);

    public abstract void setVisible(String address, boolean isVisible);
}
