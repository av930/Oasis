package com.lge.locationalarm;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import com.google.android.maps.GeoPoint;
//import com.google.android.maps.GestureDetector.OnGestureListener;
import com.google.android.maps.ItemizedOverlay;
import com.google.android.maps.MapActivity;
import com.google.android.maps.MapController;
import com.google.android.maps.MapView;
import com.google.android.maps.Overlay;
import com.google.android.maps.OverlayItem;

import android.content.Context;

import android.graphics.drawable.Drawable;
import android.location.Address;
import android.location.Criteria;
import android.location.Geocoder;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class SetLocation extends MapActivity  {
    private final String LOG_TAG = "kh";
    MapView mMapView;
    Button mButton01;
    Button mButton02;
    private LocationManager locM; //��ġ �Ŵ���
    private LocationListener locL; //��ġ ������
    private Location currentLocation; //���� ��ġ
    private String bestProvider; //���� ��ġ���� ������������ ���ι��̴�. (network, gps)
    private List<Overlay> listOfOverlays; //�ʿ� ǥ�õ� ��������(���̾�)���� ������ �ִ� ����Ʈ
    private LocationItemizedOverlay overlayHere; //������ġ ��Ŀ�� ǥ�õǾ��� ��������
    private LocationItemizedOverlay overlayBranch; //������ġ ��Ŀ���� ǥ�õǾ��� ��������
    private MapController mapController; //���� �ܽ�Ű�ų�, �̵���Ű�µ� ���� ��Ʈ�ѷ�



    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.setlocation_layout);
        mMapView = (MapView)findViewById(R.id.mapview);
        
        mMapView.setBuiltInZoomControls(true); //����,�ܾƿ� ��Ʈ���� ǥ���Ѵ�.
        mapController = mMapView.getController(); //����Ʈ�ѷ��� �����´�.
        mapController.setZoom(20); //�ʱ� Ȯ��� 17������..
//        mMapView.setSatellite(true);
        //��ġ �Ŵ����� �ý������κ��� �޾ƿ´�.
        locM = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
        //��밡���� ������ ���ι��̴��� �޾ƿ´�.
        //network (���� 3G��,Wifi AP ��ġ����)�Ǵ� gps ���� �ϳ��� �����ȴ�.
        bestProvider = locM.getBestProvider(new Criteria(), true);
        //��⿡ ������ �ִ� ������ ��ġ������ ������ġ�� �ʱ� �����Ѵ�.
        currentLocation = locM.getLastKnownLocation(bestProvider);
        //��ġ ������ �ʱ�ȭ
        locL = new MyLocationListener();
        //��ġ �Ŵ����� ��ġ �����ʸ� �����Ѵ�.
        //��ġ �����ʿ��� 10000ms (10��) ���� 100���� �̻� �̵��� �߰ߵǸ� ������Ʈ�� �Ϸ��Ѵ�.
        locM.requestLocationUpdates(bestProvider, 10000, 100, locL);
        //ó���� �ѹ� �ʺ信 �׷��ش�.
        updateOverlay(currentLocation);

        mButton01 	= (Button) findViewById(R.id.Button01);
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                MapController control = mMapView.getController();
                control.zoomIn();
            }
        });
        
        mButton02 	= (Button) findViewById(R.id.Button02);
        mButton02.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                MapController control = mMapView.getController();
                control.zoomOut();
            }
        });
    }

    @Override
    protected boolean isRouteDisplayed() {
        // TODO Auto-generated method stub
        return false;
    }

    public class MyLocationListener implements LocationListener {
        //@Override
        public void onLocationChanged(Location location) {
            //��ġ �̵��� �߰ߵǾ����� ȣ��� �޼ҵ�.
            //���� �������� 10�ʸ��� 100���� �̻� �̵��� �߰ߵǸ� ȣ��ȴ�.
            updateOverlay(location);
        }
        //@Override
        public void onProviderDisabled(String provider) {
            Log.d(LOG_TAG, "GPS disabled : " + provider);
        }
        //@Override
        public void onProviderEnabled(String provider) {
            Log.d(LOG_TAG, "GPS Enabled : " + provider);
        }
        //@Override
        public void onStatusChanged(String provider, int status, Bundle extras) {
            Log.d(LOG_TAG, "onStatusChanged : " + provider + " & status = "
            + status);
        }
    }

    protected void updateOverlay(Location location) {

        //������ ȭ�鿡 ���� �������� (��Ŀ��)�� �� �����.
        listOfOverlays = mMapView.getOverlays(); //�ʺ信�� �������� ����Ʈ�� �����´�.
        if (listOfOverlays.size() > 0) {
            listOfOverlays.clear(); //�������̰� ������ �� �����ش�.
            Log.d(LOG_TAG, "clear overlays : " + listOfOverlays.size());
        } else {
        	Log.d(LOG_TAG, "empty overlays");
        }
        //Location ��ü�� ������ GeoPoint ��ü�� ���� �޼ҵ�
        GeoPoint geoPoint = getGeoPoint(location);
        //������ġ�� ǥ���� �̹���
        Drawable marker;
        //���� ��ҽ��� �б��Ͽ� ������ġ�� ������ġ �̹����� �����ϰ� �Ǿ��ִ�.
        marker = getResources().getDrawable(R.drawable.icon);
        marker.setBounds(0, 0, marker.getIntrinsicWidth(), marker.getIntrinsicHeight());
        //LocationItemizedOverlay �� �̿��Ͽ� ������ġ ��Ŀ�� ���� �������̸� �����Ѵ�.
        overlayHere = new LocationItemizedOverlay(marker);
        //touch event �� null pointer ���׸� �����ϱ� ���� ��Ŀ�� ��� �ٷ� populate �����ش�.
        overlayHere.mPopulate();
        //������ġ�� GeoCoder �� �̿��Ͽ� �뷫�ּҿ� ��,�浵�� Toast �� ���Ͽ� �����ش�.
        String geoString;
        if(location == null)
        	geoString = "nowhere";
        else         	
        	geoString = showNowHere(location.getLatitude(), location.getLongitude() , true);
        //������ġ ��Ŀ ����
        OverlayItem overlayItem = new OverlayItem(geoPoint, "here", geoString);
        overlayHere.addOverlay(overlayItem); //������ġ �������� ����Ʈ�� ������ġ ��Ŀ�� �ִ´�.
        // ���������� HTTP����� ���� �������� �޾ƿͼ� ���������� brList (��������Ʈ)�� �ִ´�.
        // ������ ����Ͽ� ������� ������ �Ǿ��ִ�.
        // ����� ��������Ʈ �������̿� �ְ� ȭ�鿡 ����ִ� �޼ҵ�.
//        showBranchMarker(location.getLatitude(), location.getLongitude(),
//                         this.searchType, SEARCH_RANGE);
        // �ʺ信�� ��ġ�̺�Ʈ�� ���� �������̸� �߰��Ѵ�.
        // Ư�������� ���� �������� Ư�� ���� �������� ��˻��� �ϱ� ���Ͽ� ��ġ�̺�Ʈ�� �޾ƿ;��Ѵ�.
        mMapView.getOverlays().add(new MapTouchDetectorOverlay());
        // ���������� ������ �������̷��̾ �ʺ信 �߰��Ѵ�.
        mMapView.getOverlays().add(overlayHere);
        mMapView.getController().animateTo(geoPoint); //������ġ�� ȭ���� �̵��Ѵ�.
        mMapView.postInvalidate(); //�ʺ並 �ٽ� �׷��ش�.

    }

    private String showNowHere(double lat, double lng , boolean showOption) {
        StringBuilder geoString = new StringBuilder();
        try {
            Geocoder goecoder = new Geocoder(getApplicationContext(),
                                             Locale.getDefault());
            Address adr = goecoder.getFromLocation(lat,
                                                   lng, 1).get(0);
            if (adr.getLocality() != null) geoString.append(adr.getLocality()).append(" ");
            if (adr.getThoroughfare() != null) geoString.append(adr.getThoroughfare());
            if (!"".equals(geoString.toString())) geoString.append("\n\n");
        } catch (Exception e) { }
        geoString.append("���� : ").append(lat).append(" ,�浵 : ").append(lng);
        if (showOption) {
            Toast.makeText(getApplicationContext(), geoString.toString(),
                           Toast.LENGTH_SHORT).show();
        }
        return geoString.toString();
    }


    private GeoPoint getGeoPoint(Location location) {
        if (location == null) {
            return null;
        }
        Double lat = location.getLatitude() * 1E6;
        Double lng = location.getLongitude() * 1E6;
        return new GeoPoint(lat.intValue(), lng.intValue());
    }


    protected class LocationItemizedOverlay extends
                ItemizedOverlay<OverlayItem> {
        private List<OverlayItem> overlays;
        public LocationItemizedOverlay(Drawable defaultMarker) { //�������� ������
            //��Ŀ �̹����� ��� �Ʒ��κ��� ��Ŀ���� ǥ���ϴ� ����Ʈ�� �ǰ� �Ѵ�.
            super(boundCenterBottom(defaultMarker));
            overlays = new ArrayList<OverlayItem>();
        }
        @Override
        protected OverlayItem createItem(int i) {
            return overlays.get(i);
        }
        @Override
        public int size() {
            return overlays.size();
        }
        public void addOverlay(OverlayItem overlay) {
            overlays.add(overlay);
            //null pointer ���׶����� �������� ������ �߰��� ������ ���� populate ������Ѵ�.
            populate();
        }
        @Override
        protected boolean onTap(int index) {
            //��Ŀ�� �������� �߻���ų �̺�Ʈ �޼ҵ��̴�.
            if ("here".equals(overlays.get(index).getTitle())) {
                //���� ��ġ�� ��� ������ �佺Ʈ �޼����� �����ش�.
                Toast.makeText(getApplicationContext(),
                               overlays.get(index).getSnippet(), Toast.LENGTH_SHORT)
                .show();
            } else {
                //���������� ��� ���̾�α׸� ���Ͽ� ���������� �����ش�.
                //����ȭ�ɱ⡯ ��ư���� �������� ��ȭ�Ŵ� ��ɵ� �߰��Ǿ��ִ�.
                //�ʺ信 ������ �ҽ��� �ƴϾ �̰������� ǥ�� ���� �ʴ´�.
//	...
            }
            return true;
        }
        //�ܺο��� ��Ŀ�� populate �� ���ֱ� ���� �޼ҵ�.
        public void mPopulate() {
            populate();
        }
    }

//	���� ������ HTTP ����� ���� �������� �޼ҵ��̴�.
//	HTTP ��Ž� �������� ���ֱ����� ������� ������ �غô�.
//	�ٵ� �����尡 �����Ѵ�� �������� �ʴ°� ����. �߸����� �ִ°��ϱ�... -_-
    /*
    private void showBranchMarker(Double lat, Double lng, String searchType,
                                  String searchRange) {
        GetMapDataThread excuteThread = new GetMapDataThread(getMapdataHandler,
                lat, lng, searchType, searchRange);
        excuteThread.start();
    }
    */
    /*
    final Handler getMapdataHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (msg.getData().getBoolean("SUCCESS_KEY")) {  // HTTP ����� ���������� �̷��������.
                // draw branches
                Drawable branchMarker;
                int markerType = 0;
                if ("0".equals(searchType)) { //�˻����ǿ����� ��Ŀ�̹����� ����,ATM �߿� ����
                    markerType = R.drawable.icon_branch;
                } else if ("1".equals(searchType)) {
                    markerType = R.drawable.icon_atm;
                }
                branchMarker = getResources().getDrawable(markerType);
                branchMarker.setBounds(0, 0, branchMarker.getIntrinsicWidth(),
                                       branchMarker.getIntrinsicHeight());
                Double lat, lng;
                //���� ��Ŀ���� �׷��� �������̸� �غ��Ѵ�.
                overlayBranch = new LocationItemizedOverlay(branchMarker);
                overlayBranch.mPopulate();
                StringBuilder sb;
                //�ݺ����� ���鼭 ��Ŀ���� �������̿� �߰��Ѵ�.
                //���߿� ��Ŀ�� �������� ���̾�α׿� ���� ������ �����ֱ����� �����꿡 ��� ������
                //string ���� �����Ѵ�.
                              for (BranchInfoDTO d : brList) {
                                    lat = Double.parseDouble(d.getYCord()) * 1E6;
                                    lng = Double.parseDouble(d.getXCord()) * 1E6;
                                    GeoPoint branchGeoPoint = new GeoPoint(lat.intValue(),
                                                                           lng.intValue());
                                    sb = new StringBuilder();
                                    sb.append(d.getBussBrNm()).append(";")
                                    .append(d.getBussBrTelNo()).append(";")
                                    .append(d.getBussBrAdr()).append(";")
                                    .append(d.getTrscDrtm()).append(";")
                                    .append(d.getBussBrAdr2());
                                    // Create new overlay with marker at geoPoint
                                    OverlayItem overlayItem = new OverlayItem(branchGeoPoint,
                                            "branch", sb.toString());
                                    overlayBranch.addOverlay(overlayItem);
                                }
                
                //��Ŀ �������� ������ ���� �޼����� �佺Ʈ�� �����ش�.
                if (overlayBranch.size() < 1) {
                    Toast.makeText(getApplicationContext(),
                                   "�˻������ ���ų� ������ �Դϴ�.\n'�޴�'��ư�� ���� ������ �����Ͽ� �ٽ� �˻��� �ּ���.",
                                   Toast.LENGTH_LONG).show();
                }
                //���� �������̸� �ʺ� �������̿� ���������� �߰����ش�.
                if (overlayBranch != null) {
                    mMapView.getOverlays().add(overlayBranch);
                    mMapView.postInvalidate();
                }
            }
        }
    };
    */
    /*
    private class GetMapDataThread extends Thread {
        private Handler tHandler;
        private Double lat, lng;
        private String searchType;
        private String searchRange;
        public GetMapDataThread(Handler tHandler) {
            this.tHandler = tHandler;
        }
        public GetMapDataThread(Handler tHandler, Double lat, Double lng,
                                String searhType, String searchRange) {
            this(tHandler); //������ ó�� �Ϸ��� ������ ������ ���������� ������ ��Ŀ�� ����� �ڵ鷯
            this.lat = lat; //����
            this.lng = lng; //�浵
            this.searchType = searhType; //�˻����� (0 : ����, 1: ATM)
            this.searchRange = searchRange; //�˻����� ������ m(����)�̴�.
        }
        @Override
        public void run() { //������ ����~
            Bundle bundle = new Bundle();
            try {
                //���������� ������ ���� ����Ʈ�� �غ��Ѵ�. BranchInfoDTO �� �������̴�.
                brList = new ArrayList<BranchInfoDTO>();
                brList = gdA.getMapData(lat.toString(), lng.toString(),
                                        searchType, searchRange);
                //gdA Ŭ������ HTTP ����� �ؼ� ���������� �������� Ŭ�����̴�.
                //���⼭�� �������� �ʾҴ�. onCreate ���� �����ߴ�.
                bundle.putBoolean("SUCCESS_KEY", true); //�����ϸ� ���鿡 �����޼��� ����
            } catch (Exception e) {
//		...
                bundle.putBoolean("SUCCESS_KEY", false); //�����ϸ� false �̴�.
                // ignore
            } finally {
                try {
                    Message msg = tHandler.obtainMessage();
                    msg.setData(bundle);
                    tHandler.sendMessage(msg); //�ڵ鷯�� �޼����� ������.
                    interrupt();
                } catch (Exception e) {
                    // ignore
                }
            }
        }
    }
*/
    
    public class MapTouchDetectorOverlay extends Overlay implements
                OnGestureListener {
        private GestureDetector gestureDetector;
        //onTouchEvent �� ACTION_DOWN ���� ������ ���� ó�� ���� �ʰ�
        //����ó���� ���� ĳġ�Ҽ��ִ� �������̴�.
        private OnGestureListener onGestureListener;
        private static final long LOOOOONG_PRESS_MILLI_SEC = 1500; // 1.5�������� ��Դ������� �ν��Ѵ�.
        // for touch timer
        private Handler mHandler;
        private long touchStartTime;
        private long longPressTime;
        private MotionEvent globalEvent;
        //������
        public MapTouchDetectorOverlay() {
            gestureDetector = new GestureDetector(this);
            init();
        }
        public MapTouchDetectorOverlay(OnGestureListener onGestureListener) {
            this();
            setOnGestureListener(onGestureListener);
            init();
        }
        
        //�����ڵ��� ȣ���� �ʱ�ȭ �Լ�
        private void init() {
            mHandler = new Handler();
            globalEvent = null;
        }
        //��Դ����� ������ ������
        private Runnable looongPressDetector = new Runnable() {
            public void run() {
                //ȭ���� ������ �ִ� �ð�
                long touchHoldTime = longPressTime - touchStartTime;
                if ((globalEvent != null)
                && (touchHoldTime > (LOOOOONG_PRESS_MILLI_SEC - 200))) { //�����߿� 200ms �� ���� �˻��ϴ°��� ��⸶�� ������ �޶� �ణ�� ������ �ذ��̴�.
                    Log.d(LOG_TAG, "loooooong press detected!");
                    float x = globalEvent.getX();
                    float y = globalEvent.getY(); //ȭ�鿡�� �����ִ� ������ �޾ƿ´�.
                    GeoPoint p = mMapView.getProjection().fromPixels((int) x,
                                 (int) y); //�����ִ� ������ ���� �浵�� �ٲ��ش�.
                    Location selectedLocation = new Location(currentLocation);
                    selectedLocation.setLatitude((p.getLatitudeE6() / 1E6));
                    selectedLocation.setLongitude((p.getLongitudeE6() / 1E6));
                    currentLocation = selectedLocation;
                    locM.removeUpdates(locL); //������ġ �����ʸ� ��� ���ֹ�����.
                    updateOverlay(currentLocation); //���� ��˻� �� ��Ŀ �ٽ� ǥ��
                    showNowHere((p.getLatitudeE6() / 1E6) , (p.getLongitudeE6() / 1E6) , true);
                }
            }
        };
        @Override
    	public boolean onTouchEvent(MotionEvent event, MapView mapView) {
        	if ( event.getAction() == MotionEvent.ACTION_DOWN ) {
//        		Toast.makeText(getBaseContext(), "HI", Toast.LENGTH_LONG).show();
        	}
//    		Toast.makeText(getBaseContext(), "aa", Toast.LENGTH_LONG).show();
        	if (gestureDetector.onTouchEvent(event)) {
        		return true;
        	}
        	onLongPress(event);
    		return false;
    	}
		//@Override
		public boolean onDown(MotionEvent e) {
			if (onGestureListener != null) {
				return onGestureListener.onDown(e);
			} else {
				// start timer
				touchStartTime = System.currentTimeMillis();
				mHandler.postDelayed(looongPressDetector,
				LOOOOONG_PRESS_MILLI_SEC);
				//1.5�� �ִٰ� ��Դ����� üũ�غ���.
			}
			return false;
		}
		//@Override
		public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX,
				float velocityY) {
			if (onGestureListener != null) {
				return onGestureListener.onFling(e1, e2, velocityX, velocityY);
			}
			return false;
		}
		//@Override
		public void onLongPress(MotionEvent e) {
			// TODO Auto-generated method stub
			if (onGestureListener != null) {
				onGestureListener.onLongPress(e);
			}
			//ȭ���� ������ ������ onLongPress �� ȣ��Ǵµ� ȣ��ɶ����� üũ�� �ð��� ������ �ִ´�.
			//�̺κ��� �����ս� �϶��� ������ �� �� ����.
			globalEvent = e;
			longPressTime = System.currentTimeMillis();
			Toast.makeText(getBaseContext(), "long press", Toast.LENGTH_LONG).show();			
		}
		//@Override
		public boolean onScroll(MotionEvent e1, MotionEvent e2,
				float distanceX, float distanceY) {
			// TODO Auto-generated method stub
			return false;
		}
		//@Override
		public void onShowPress(MotionEvent e) {
			// TODO Auto-generated method stub
			if (onGestureListener != null) {
				onGestureListener.onShowPress(e);
			}			
		}
		//@Override
		public boolean onSingleTapUp(MotionEvent e) {
			if (onGestureListener != null) {
				onGestureListener.onSingleTapUp(e);
			}
			return false;
		}
        public void setOnGestureListener(OnGestureListener onGestureListener) {
        	this.onGestureListener = onGestureListener;
        }		
    }
}